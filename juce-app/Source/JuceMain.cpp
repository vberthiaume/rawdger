#include <juce_gui_basics/juce_gui_basics.h>
#include "MainWindow.h"

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
