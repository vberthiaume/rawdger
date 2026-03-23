#include "rawdger/Recorder.h"

namespace rawdger
{

void Recorder::startRecording()
{
    // release: ensures any prior writes (e.g. buffer clear) are visible
    // to the audio thread before it sees recording == true.
    recording.store(true, std::memory_order_release);
}

void Recorder::stopRecording()
{
    recording.store(false, std::memory_order_release);
}

bool Recorder::isRecording() const
{
    return recording.load(std::memory_order_acquire);
}

void Recorder::processBlock(std::span<const float> samples)
{
    // acquire: pairs with the release in start/stopRecording() so we
    // always see the most recent state of the flag.
    if (!recording.load(std::memory_order_acquire))
        return;

    // TODO: Replace with lock-free FIFO when we add the writer thread.
    // For now, direct vector append is fine for testing the interface.
    // This is NOT real-time-safe (vector may allocate), but lets us
    // validate the processBlock contract before adding the FIFO.
    capturedSamples.insert(capturedSamples.end(), samples.begin(), samples.end());
}

std::vector<float> Recorder::takeRecordedSamples()
{
    // std::move transfers ownership of the internal buffer to the caller,
    // leaving capturedSamples in a valid but empty state.
    return std::move(capturedSamples);
}

} // namespace rawdger
