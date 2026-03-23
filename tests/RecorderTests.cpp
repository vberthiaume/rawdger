// Unit tests for rawdger::Recorder.
// Each TEST_CASE gets a fresh Recorder instance — no shared state between tests.
// Run with: ./build/tests/rawdger-tests (or via CTest / CLion's test runner).

#include <catch2/catch_test_macros.hpp>
#include "rawdger/Recorder.h"

TEST_CASE("Recorder starts in non-recording state")
{
    rawdger::Recorder recorder;
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("Recorder can start and stop recording")
{
    rawdger::Recorder recorder;

    recorder.startRecording();
    REQUIRE(recorder.isRecording());

    recorder.stopRecording();
    REQUIRE_FALSE(recorder.isRecording());
}

TEST_CASE("processBlock captures samples while recording")
{
    rawdger::Recorder recorder;
    const float samples[] = { 0.1f, 0.2f, 0.3f };

    recorder.startRecording();
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
    rawdger::Recorder recorder;
    const float block1[] = { 1.0f, 2.0f };
    const float block2[] = { 3.0f, 4.0f };

    // Simulate two consecutive audio callbacks while recording.
    recorder.startRecording();
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
    rawdger::Recorder recorder;
    const float samples[] = { 1.0f };

    recorder.startRecording();
    recorder.processBlock({ samples, 1 });
    recorder.stopRecording();

    // First call should return the captured data.
    auto first = recorder.takeRecordedSamples();
    REQUIRE(first.size() == 1);

    // Second call should return empty — the buffer was moved out.
    auto second = recorder.takeRecordedSamples();
    REQUIRE(second.empty());
}
