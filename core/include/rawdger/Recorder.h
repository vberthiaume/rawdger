#pragma once

#include <atomic>
#include <cstdint>
#include <span>
#include <vector>

namespace rawdger
{

/// Core recording engine, shared by both the JUCE and Qt apps.
/// The UI layer calls startRecording()/stopRecording(), while the real-time
/// audio callback feeds samples in via processBlock().
class Recorder
{
public:
    /// These toggle the recording state via an atomic flag, so they are safe
    /// to call from the UI thread while processBlock() runs on the audio thread.
    void startRecording();
    void stopRecording();
    bool isRecording() const;

    /// Called from the real-time audio thread on each audio callback.
    /// When recording is active, samples are appended to an internal buffer.
    /// When recording is inactive, this is a no-op.
    void processBlock(std::span<const float> samples);

    /// Returns all captured samples and clears the internal buffer via move.
    /// NOT thread-safe — only call this when recording is stopped.
    std::vector<float> takeRecordedSamples();

private:
    /// Atomic flag checked by processBlock() on the audio thread and toggled
    /// by start/stopRecording() on the UI thread. Uses acquire/release
    /// ordering to ensure visibility across threads without a mutex.
    std::atomic<bool> recording { false };

    std::vector<float> capturedSamples;
};

} // namespace rawdger
