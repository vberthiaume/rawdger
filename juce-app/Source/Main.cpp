#include <juce_core/juce_core.h>
#include "rawdger/Core.h"

int main()
{
    juce::Logger::writeToLog ("JUCE app says: " + juce::String (rawdger::getString()));
    return 0;
}
