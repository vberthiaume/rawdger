#include <juce_core/juce_core.h>
#include "rawdger/Core.h"
#include <iostream>

struct StdOutLogger : public juce::Logger
{
    void logMessage (const juce::String& message) override
    {
        std::cout << message << std::endl;
    }
};

int main()
{
    StdOutLogger logger;
    juce::Logger::setCurrentLogger (&logger);

    juce::Logger::writeToLog ("JUCE app says: " + juce::String (rawdger::getString()));
    DBG("now with DBG JUCE app says: " + juce::String(rawdger::getString()));

    juce::Logger::setCurrentLogger (nullptr);
    return 0;
}
