#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "rawdger/Core.h"
#include <iostream>

struct StdOutLogger : public juce::Logger
{
    void logMessage (const juce::String& message) override
    {
        std::cout << message << std::endl;
    }
};

class MainComponent : public juce::Component,
                      public juce::AudioIODeviceCallback
{
public:
    MainComponent()
        : backgroundThread ("Audio Recording Thread")
    {
        recordButton.setButtonText ("Record");
        recordButton.setClickingTogglesState (true);
        recordButton.onClick = [this]
        {
            if (recordButton.getToggleState())
                startRecording();
            else
                stopRecording();
        };
        addAndMakeVisible (recordButton);

        setSize (300, 200);

        backgroundThread.startThread();

        auto result = deviceManager.initialise (2, 0, nullptr, true);
        if (result.isNotEmpty())
            juce::Logger::writeToLog ("Audio device error: " + result);

        deviceManager.addAudioCallback (this);
    }

    ~MainComponent() override
    {
        stopRecording();
        deviceManager.removeAudioCallback (this);
        backgroundThread.stopThread (4000);
    }

    void resized() override
    {
        recordButton.setBounds (getLocalBounds().reduced (50));
    }

    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext&) override
    {
        if (auto* writer = activeWriter.load())
            writer->write (inputChannelData, numSamples);

        for (int i = 0; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
    }

    void audioDeviceAboutToStart (juce::AudioIODevice* device) override
    {
        sampleRate = device->getCurrentSampleRate();
        numChannels = device->getActiveInputChannels().countNumberOfSetBits();
        if (numChannels == 0)
            numChannels = 1;
    }

    void audioDeviceStopped() override {}

private:
    void startRecording()
    {
        auto file = getNextRecordingFile();
        file.getParentDirectory().createDirectory();

        juce::Logger::writeToLog ("Recording to: " + file.getFullPathName());

        std::unique_ptr<juce::OutputStream> fileStream (file.createOutputStream());
        if (fileStream != nullptr)
        {
            juce::WavAudioFormat wavFormat;
            auto options = juce::AudioFormatWriterOptions{}
                               .withSampleRate (sampleRate)
                               .withNumChannels (numChannels)
                               .withBitsPerSample (16);

            auto writer = wavFormat.createWriterFor (fileStream, options);
            if (writer != nullptr)
            {
                threadedWriter.reset (new juce::AudioFormatWriter::ThreadedWriter (writer.release(), backgroundThread, 32768));
                activeWriter.store (threadedWriter.get());
            }
        }
    }

    void stopRecording()
    {
        activeWriter.store (nullptr);
        threadedWriter.reset();
        juce::Logger::writeToLog ("Recording stopped.");
    }

    juce::File getNextRecordingFile() const
    {
        auto timestamp = juce::Time::getCurrentTime().formatted ("%Y-%m-%d_%H%M%S");
        auto documentsDir = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory);
        return documentsDir.getChildFile ("Rawdger").getChildFile ("rawdger_" + timestamp + ".wav");
    }

    juce::TextButton recordButton;
    juce::AudioDeviceManager deviceManager;
    juce::TimeSliceThread backgroundThread;
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
    double sampleRate = 44100.0;
    int numChannels = 1;
};

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow()
        : DocumentWindow ("Rawdger JUCE", juce::Colours::darkgrey, allButtons)
    {
        setContentOwned (new MainComponent(), true);
        centreWithSize (getWidth(), getHeight());
        setVisible (true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

class RawdgerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return "Rawdger JUCE"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }

    void initialise (const juce::String&) override
    {
        logger = std::make_unique<StdOutLogger>();
        juce::Logger::setCurrentLogger (logger.get());
        mainWindow = std::make_unique<MainWindow>();
    }

    void shutdown() override
    {
        mainWindow = nullptr;
        juce::Logger::setCurrentLogger (nullptr);
    }

    void systemRequestedQuit() override
    {
        quit();
    }

private:
    std::unique_ptr<StdOutLogger> logger;
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (RawdgerApplication)
