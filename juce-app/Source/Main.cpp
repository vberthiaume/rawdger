#include <juce_gui_basics/juce_gui_basics.h>
#include "rawdger/Core.h"
#include <iostream>

struct StdOutLogger : public juce::Logger
{
    void logMessage (const juce::String& message) override
    {
        std::cout << message << std::endl;
    }
};

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
                juce::Logger::writeToLog ("on");
            else
                juce::Logger::writeToLog ("off");
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
