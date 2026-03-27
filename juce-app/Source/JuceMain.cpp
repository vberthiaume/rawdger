#include <juce_gui_basics/juce_gui_basics.h>
#include "rawdger/Recorder.h"

class MainComponent : public juce::Component
{
public:
    MainComponent()
    {
        recordButton.setButtonText ("Record");
        recordButton.setClickingTogglesState (true);
        recordButton.onClick = [this]
        {
            if (recordButton.getToggleState())
            {
                auto path = juce::File::getSpecialLocation (juce::File::userDesktopDirectory)
                                .getChildFile (rawdger::generateWavFileName ("juce"))
                                .getFullPathName()
                                .toStdString();
                recorder.startRecording (path);
                DBG ("Recording to: " + juce::String (path));
            }
            else
            {
                recorder.stopRecording();
                DBG ("Recording stopped.");
            }
        };
        addAndMakeVisible (recordButton);

        setSize (300, 200);
    }

    void resized() override
    {
        recordButton.setBounds (getLocalBounds().reduced (50));
    }

private:
    juce::TextButton recordButton;
    rawdger::Recorder recorder;
};

//======================================================================================

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

//======================================================================================

class RawdgerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return "Rawdger JUCE"; }
    const juce::String getApplicationVersion() override { return "0.1.0"; }

    void initialise (const juce::String&) override
    {
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
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (RawdgerApplication)
