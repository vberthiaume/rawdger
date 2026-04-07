#pragma once

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
    juce::TextButton  recordButton;
    rawdger::Recorder recorder;
};
