#pragma once

#include <atomic>
#include <fstream>
#include <span>
#include <string>
#include <vector>

namespace rawdger
{

/** Generates a timestamped WAV filename, e.g. "rawdger_juce_2026-03-27_14-30-45.wav".
 *  @param appName  Identifier for the calling app (e.g. "juce" or "qt").
 */
std::string generateWavFileName (const std::string& appName);

/** Core recording engine, shared by both the JUCE and Qt apps.
 *  The UI layer calls startRecording()/stopRecording(), while the real-time
 *  audio callback feeds samples in via processBlock().
 */
class Recorder
{
  public:
    /** Creates a WAV file at the given path, writes a valid header, and sets
     *  the recording flag. The header uses placeholder sizes that get patched
     *  when stopRecording() is called.
     *  @param filePath  Absolute path for the output WAV file.
     */
    void startRecording (const std::string& filePath);

    /** Finalizes the WAV header (patches the RIFF and data chunk sizes),
     *  closes the file, and clears the recording flag.
     */
    void stopRecording();

    /** @return true if recording is currently active. */
    bool isRecording() const;

    /** Called from the real-time audio thread on each audio callback.
     *  When recording is active, samples are appended to an internal buffer.
     *  When recording is inactive, this is a no-op.
     *  @param samples  Audio samples from the current callback.
     */
    void processBlock (std::span<const float> samples);

    /** Returns all captured samples and clears the internal buffer via move.
     *  NOT thread-safe — only call this when recording is stopped.
     *  @return The captured sample buffer (moved out).
     */
    std::vector<float> takeRecordedSamples();

    /** @return The path of the currently open (or most recently closed) WAV file. */
    std::string getFilePath() const;

  private:
    void writeWavHeader();
    void finalizeWavHeader();

    /// Atomic flag checked by processBlock() on the audio thread and toggled
    /// by start/stopRecording() on the UI thread. Uses acquire/release
    /// ordering to ensure visibility across threads without a mutex.
    std::atomic<bool> recording { false };

    std::vector<float> capturedSamples;

    std::ofstream wavFile;
    std::string   currentFilePath;

    // WAV format defaults — will be parameterized when audio input is added.
    static constexpr uint32_t sampleRate    = 44100;
    static constexpr uint16_t numChannels   = 1;
    static constexpr uint16_t bitsPerSample = 16;
};

} // namespace rawdger
