#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MainComponent.h"

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
