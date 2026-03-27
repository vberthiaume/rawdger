// Unit tests for rawdger::Recorder.
// Each TEST_CASE gets a fresh Recorder instance — no shared state between tests.
// Run with: ./build/tests/rawdger-tests (or via CTest / CLion's test runner).

#include <catch2/catch_test_macros.hpp>
#include "rawdger/Recorder.h"

#include <filesystem>
#include <fstream>
#include <string_view>

// Helper: returns a unique temp file path and ensures cleanup after the test.
struct TempWav
{
    std::string path;
    TempWav() : path(std::filesystem::temp_directory_path() / "rawdger_test.wav") {}
    ~TempWav() { std::filesystem::remove(path); }
};

// Helper: reads the first N bytes of a file into a vector.
static std::vector<char> readBytes(const std::string& path, size_t count)
{
    std::ifstream f(path, std::ios::binary);
    std::vector<char> buf(count);
    f.read(buf.data(), static_cast<std::streamsize>(count));
    buf.resize(static_cast<size_t>(f.gcount()));
    return buf;
}

// Helper: reads a little-endian value of type T from a byte buffer at the given offset.
template <typename T>
static T readLE(const std::vector<char>& buf, size_t offset)
{
    T value {};
    std::copy_n(buf.data() + offset, sizeof(T), reinterpret_cast<char*>(&value));
    return value;
}

TEST_CASE("generateWavFileName contains app name and .wav extension")
{
    auto name = rawdger::generateWavFileName("juce");
    REQUIRE(name.starts_with("rawdger_juce_"));
    REQUIRE(name.ends_with(".wav"));
}

TEST_CASE("Recorder starts in non-recording state")
{
    rawdger::Recorder recorder;
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("Recorder can start and stop recording")
{
    TempWav tmp;
    rawdger::Recorder recorder;

    recorder.startRecording(tmp.path);
    REQUIRE(recorder.isRecording());

    recorder.stopRecording();
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("startRecording creates a valid WAV file with a 44-byte header")
{
    TempWav tmp;
    rawdger::Recorder recorder;

    recorder.startRecording(tmp.path);
    recorder.stopRecording();

    REQUIRE(std::filesystem::exists(tmp.path));

    auto header = readBytes(tmp.path, 44);
    REQUIRE(header.size() == 44);

    auto tag = [&](size_t offset, size_t len) {
        return std::string_view(header.data() + offset, len);
    };

    // RIFF header
    REQUIRE(tag(0, 4) == "RIFF");
    REQUIRE(tag(8, 4) == "WAVE");

    // fmt chunk
    REQUIRE(tag(12, 4) == "fmt ");

    // Audio format: PCM (1)
    REQUIRE(readLE<uint16_t>(header, 20) == 1);

    // Channels: 1
    REQUIRE(readLE<uint16_t>(header, 22) == 1);

    // Sample rate: 44100
    REQUIRE(readLE<uint32_t>(header, 24) == 44100);

    // Bits per sample: 16
    REQUIRE(readLE<uint16_t>(header, 34) == 16);

    // data chunk
    REQUIRE(tag(36, 4) == "data");
}

TEST_CASE("WAV header sizes are correct for an empty recording")
{
    TempWav tmp;
    rawdger::Recorder recorder;

    recorder.startRecording(tmp.path);
    recorder.stopRecording();

    auto header = readBytes(tmp.path, 44);

    // Data chunk size should be 0 (no audio written yet).
    REQUIRE(readLE<uint32_t>(header, 40) == 0);

    // RIFF chunk size should be 36 (header minus 8, plus 0 data bytes).
    REQUIRE(readLE<uint32_t>(header, 4) == 36);
}

TEST_CASE("stopRecording is safe to call when not recording")
{
    rawdger::Recorder recorder;
    // Should not throw or crash.
    recorder.stopRecording();
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("getFilePath returns the path passed to startRecording")
{
    TempWav tmp;
    rawdger::Recorder recorder;

    recorder.startRecording(tmp.path);
    REQUIRE(recorder.getFilePath() == tmp.path);
    recorder.stopRecording();
}

TEST_CASE("processBlock captures samples while recording")
{
    TempWav tmp;
    rawdger::Recorder recorder;
    const float samples[] = { 0.1f, 0.2f, 0.3f };

    recorder.startRecording(tmp.path);
    recorder.processBlock({ samples, 3 });
    recorder.stopRecording();

    auto captured = recorder.takeRecordedSamples();
    REQUIRE(captured.size() == 3);
    REQUIRE(captured[0] == 0.1f);
    REQUIRE(captured[1] == 0.2f);
    REQUIRE(captured[2] == 0.3f);
}

TEST_CASE("processBlock ignores samples when not recording")
{
    rawdger::Recorder recorder;
    const float samples[] = { 0.1f, 0.2f };

    // processBlock should be a no-op when recording hasn't been started.
    recorder.processBlock({ samples, 2 });

    auto captured = recorder.takeRecordedSamples();
    REQUIRE(captured.empty());
}

TEST_CASE("processBlock accumulates across multiple calls")
{
    TempWav tmp;
    rawdger::Recorder recorder;
    const float block1[] = { 1.0f, 2.0f };
    const float block2[] = { 3.0f, 4.0f };

    // Simulate two consecutive audio callbacks while recording.
    recorder.startRecording(tmp.path);
    recorder.processBlock({ block1, 2 });
    recorder.processBlock({ block2, 2 });
    recorder.stopRecording();

    auto captured = recorder.takeRecordedSamples();
    REQUIRE(captured.size() == 4);
    REQUIRE(captured[0] == 1.0f);
    REQUIRE(captured[3] == 4.0f);
}

TEST_CASE("takeRecordedSamples clears the buffer")
{
    TempWav tmp;
    rawdger::Recorder recorder;
    const float samples[] = { 1.0f };

    recorder.startRecording(tmp.path);
    recorder.processBlock({ samples, 1 });
    recorder.stopRecording();

    // First call should return the captured data.
    auto first = recorder.takeRecordedSamples();
    REQUIRE(first.size() == 1);

    // Second call should return empty — the buffer was moved out.
    auto second = recorder.takeRecordedSamples();
    REQUIRE(second.empty());
}
