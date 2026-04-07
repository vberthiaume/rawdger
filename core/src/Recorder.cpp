#include "rawdger/Recorder.h"

#include <chrono>
#include <sstream>
#include <stdexcept>

namespace rawdger
{

std::string generateWavFileName (const std::string& appName)
{
    auto               now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now());
    std::ostringstream oss;
    oss << "rawdger_" << appName << "_"
        << std::put_time (std::localtime (&now), "%Y-%m-%d_%H-%M-%S")
        << ".wav";
    return oss.str();
}

void Recorder::startRecording (const std::string& filePath)
{
    currentFilePath = filePath;

    wavFile.open (filePath, std::ios::binary | std::ios::trunc);
    if (! wavFile.is_open())
        throw std::runtime_error ("Failed to create WAV file: " + filePath);

    writeWavHeader();

    // release: ensures all header writes are visible to the audio thread
    // before it sees recording == true.
    recording.store (true, std::memory_order_release);
}

void Recorder::stopRecording()
{
    recording.store (false, std::memory_order_release);

    if (wavFile.is_open())
    {
        finalizeWavHeader();
        wavFile.close();
    }
}

bool Recorder::isRecording() const
{
    return recording.load (std::memory_order_acquire);
}

void Recorder::processBlock (std::span<const float> samples)
{
    // acquire: pairs with the release in start/stopRecording() so we
    // always see the most recent state of the flag.
    if (! recording.load (std::memory_order_acquire))
        return;

    // TODO: Replace with lock-free FIFO when we add the writer thread.
    // For now, direct vector append is fine for testing the interface.
    // This is NOT real-time-safe (vector may allocate), but lets us
    // validate the processBlock contract before adding the FIFO.
    capturedSamples.insert (capturedSamples.end(), samples.begin(), samples.end());
}

std::vector<float> Recorder::takeRecordedSamples()
{
    // std::move transfers ownership of the internal buffer to the caller,
    // leaving capturedSamples in a valid but empty state.
    return std::move (capturedSamples);
}

std::string Recorder::getFilePath() const
{
    return currentFilePath;
}

// Writes a standard 44-byte WAV header with placeholder sizes.
// The RIFF chunk size (offset 4) and data chunk size (offset 40) are
// written as 0 and get patched by finalizeWavHeader() once we know the
// actual amount of audio data.
void Recorder::writeWavHeader()
{
    auto write16 = [this] (uint16_t value)
    { wavFile.write (reinterpret_cast<const char*> (&value), 2); };
    auto write32 = [this] (uint32_t value)
    { wavFile.write (reinterpret_cast<const char*> (&value), 4); };

    uint16_t blockAlign = numChannels * (bitsPerSample / 8);
    uint32_t byteRate   = sampleRate * blockAlign;

    wavFile.write ("RIFF", 4);
    write32 (0); // placeholder for RIFF chunk size
    wavFile.write ("WAVE", 4);

    wavFile.write ("fmt ", 4);
    write32 (16); // fmt chunk size (PCM)
    write16 (1);  // audio format: PCM
    write16 (numChannels);
    write32 (sampleRate);
    write32 (byteRate);
    write16 (blockAlign);
    write16 (bitsPerSample);

    wavFile.write ("data", 4);
    write32 (0); // placeholder for data chunk size
}

// Seeks back into the header to patch the two size fields now that
// we know how much audio data was written (if any).
void Recorder::finalizeWavHeader()
{
    // The data chunk size is (current position - 44), since the header is
    // exactly 44 bytes and all audio data follows immediately after.
    auto     endPos   = wavFile.tellp();
    uint32_t dataSize = static_cast<uint32_t> (endPos) - 44;
    uint32_t riffSize = dataSize + 36; // 36 = header bytes after the RIFF size field

    // Patch RIFF chunk size at offset 4.
    wavFile.seekp (4);
    wavFile.write (reinterpret_cast<const char*> (&riffSize), 4);

    // Patch data chunk size at offset 40.
    wavFile.seekp (40);
    wavFile.write (reinterpret_cast<const char*> (&dataSize), 4);
}

} // namespace rawdger
