#include "gui/MainComponent.h"
#include "Amiga.h"

class amigaMonApplication final : public juce::JUCEApplication
{
public:
    amigaMonApplication()
    {
        amiga.getAmiga().set(OPT_MEM_CHIP_RAM, 512);
        amiga.getAmiga().set(OPT_MEM_SLOW_RAM, 512);
        amiga.getAmiga().set(OPT_HOST_REFRESH_RATE, 50);
    };

    const juce::String getApplicationName() override       { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override    { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override             { return false; }

    void initialise (const juce::String& commandLine) override
    {
        juce::ignoreUnused (commandLine);

        amiga.initialiseGUI(getApplicationName(), amiga);
    }

    void shutdown() override
    {
        amiga.shutdownGUI();
    }

    void systemRequestedQuit() override
    {
        if(amiga.canQuit()) quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        juce::ignoreUnused (commandLine);
    }

private:
    amigaMon::Amiga amiga;
};

START_JUCE_APPLICATION (amigaMonApplication)