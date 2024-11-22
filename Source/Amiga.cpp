//
// Created by Rich Elmes on 15.11.2024.
//

#include "Amiga.h"

#include <Emulator.h>
#include <gui/ScreenSizeComponent.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include <utility>
#include "Window.h"
#include "gui/MainComponent.h"
#include "gui/ControlsComponent.h"

namespace amigaMon {
    inline static Amiga* instance = nullptr;

    Amiga::Amiga()
    {
        instance = this;
        vAmiga.emu->initialize();

        audioDeviceManager.addAudioCallback(this);
        audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);

        loadSettings();
    }

    Amiga::~Amiga()
    {
        audioDeviceManager.removeAudioCallback(this);
    }

    void Amiga::initialiseGUI(juce::String name, amigaMon::Amiga& amiga)
    {
        using namespace amigaMon;
        mainWindow = std::make_unique<Window<MainComponent>>(name, amiga);
        controlsWindowConstrainer = std::make_unique<juce::ComponentBoundsConstrainer>();
        controlsWindowConstrainer->setSizeLimits(400, static_cast<int>(400 * (1.0 / aspectRatio)),
                                                 800, static_cast<int>(800 * (1.0 / aspectRatio)));
        controlsWindowConstrainer->setFixedAspectRatio(aspectRatio);
        controlsWindow = std::make_unique<Window<ControlsComponent>>("Controls", amiga,
                                                                     Window<ControlsComponent>::resizable,
                                                                     controlsWindowConstrainer.get());
        auto display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
        mainWindow->setTopLeftPosition(display->userArea.getCentre().getX() - mainWindow->getWidth(),
                                       display->userArea.getCentre().getY() - mainWindow->getHeight());
        controlsWindow->setTopLeftPosition(mainWindow->getRight(), mainWindow->getY());
    }

    void Amiga::shutdownGUI()
    {
        mainWindow.reset();
        controlsWindow.reset();
    }

    bool Amiga::canQuit()
    {
        vAmiga.emu->resume();
        return true;
    }

    void Amiga::start()
    {
        auto romTraits = vAmiga.mem.getRomTraits();
        if(romTraits.crc != 0)
        {
            vAmiga.launch(this, &Amiga::callback);
        }
    }

    void Amiga::loadDiskAndReboot(const std::string &path, bool reboot)
    {
        if(vAmiga.df0.drive->hasDisk())
        {
            vAmiga.df0.ejectDisk();
        }
        auto disk = vamiga::FloppyFile::make(std::filesystem::path(path));
        vAmiga.df0.insertMedia(*disk, true);
        if(reboot)
        {
            vAmiga.emu->hardReset();
            if(vAmiga.emu->isSuspended())
            {
                vAmiga.emu->resume();
            }
        }
    }

    vamiga::VAmiga& Amiga::getAmiga()
    {
        return vAmiga;
    }

    void Amiga::audioDeviceAboutToStart(juce::AudioIODevice *device)
    {
        sampleRate = device->getCurrentSampleRate();
    }

    void Amiga::audioDeviceStopped()
    {
        sampleRate = 0;
    }

    void Amiga::audioDeviceIOCallbackWithContext(const float * const */*inputChannelData*/, int /*numInputChannels*/,
                                                 float * const *outputChannelData, int numOutputChannels,
                                                 int numSamples, const juce::AudioIODeviceCallbackContext &context)
    {
        ignoreUnused (context);

        constexpr auto leftChannel = 0;
        constexpr auto rightChannel = 1;

        vAmiga.audioPort.copyStereo(outputChannelData[leftChannel], outputChannelData[rightChannel], numSamples);

        for(int index = 0; index < numSamples; ++index)
        {
            auto left = outputChannelData[leftChannel][index];
            auto right = outputChannelData[rightChannel][index];
            auto gain = 1.0f;
            if(audioMixFactor.load() > 0.5f)
            {
                gain = 1.0f - (audioMixFactor.load() - 0.5f) * 0.6f;
            }
            outputChannelData[leftChannel][index]  = (left + audioMixFactor * right) * gain;
            outputChannelData[rightChannel][index] = (right + audioMixFactor * left) * gain;
        }

        for (int i = 2; i < numOutputChannels; ++i)
            if (outputChannelData[i] != nullptr)
                juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
    }

    juce::File Amiga::getStartDirectory()
    {
        return startDirectory;
    }

    void Amiga::advanceFrame()
    {
        shouldAdvanceFrame.store(true);
    }

    int Amiga::getDisplayOffsetX() const
    {
        return displayOffsetX.load();;
    }

    int Amiga::getDisplayOffsetY() const
    {
        return displayOffsetY.load();
    }

    void Amiga::setDisplayOffsetX(int newDisplayOffsetX)
    {
        displayOffsetX.store(newDisplayOffsetX);
        sendChangeMessage();
    }

    void Amiga::setDisplayOffsetY(int newDisplayOffsetY)
    {
        displayOffsetY.store(newDisplayOffsetY);
        sendChangeMessage();
    }

    int Amiga::getDisplayWidth() const
    {
        return displayWidth.load();
    }

    int Amiga::getDisplayHeight() const
    {
        return displayHeight.load();
    }

    void Amiga::setDisplayWidth(int newDisplayWidth)
    {
        displayWidth.store(newDisplayWidth);
        sendChangeMessage();
    }

    void Amiga::setDisplayHeight(int newDisplayHeight)
    {
        displayHeight.store(newDisplayHeight);
        sendChangeMessage();
    }

    void Amiga::setStartDirectory(juce::File directory)
    {
        startDirectory = directory;
    }

    void Amiga::showDisplaySettings()
    {
        // show ScreenSizeComponent in a juce::DocumentWindow
        using namespace amigaMon;

        auto window = std::make_unique<Window<ScreenSizeComponent>>("Display Settings", *this);
        window->onCloseButtonPressed = [this]() {
            screenSizeWindow.reset();
        };
        screenSizeWindow = std::move(window);
    }

    float Amiga::getAudioMixFactor() const
    {
        return audioMixFactor.load();
    }

    void Amiga::setAudioMixFactor(float newAudioMixFactor)
    {
        audioMixFactor.store(newAudioMixFactor);
    }

    void Amiga::showSaveSettingsPopup()
    {
        juce::PopupMenu menu;
        menu.addItem("Save Settings", [this]() {
            saveSettings();
        });
        menu.showMenuAsync(juce::PopupMenu::Options());
    }

    void Amiga::callback(const void *, Message message)
    {
        if(instance == nullptr)
            return;
        auto& vAmiga = instance->vAmiga;

        if(message.type == MSG_RSH_UPDATE)
        {
            vAmiga.emu->run();
        }
    }

    int Amiga::getSizeMultiply() const
    {
        return sizeMultiply.load();
    }

    void Amiga::setSizeMultiply(int newSizeMultiply)
    {
        sizeMultiply.store(newSizeMultiply);
        sendChangeMessage();
    }

    juce::File Amiga::getSettingsFolder()
    {
        auto settingsFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
#if JUCE_MAC
        .getChildFile("Application Support")
#endif
        .getChildFile("AmigaMon");
        if(!settingsFolder.exists())
        {
            settingsFolder.createDirectory();
        }
        return settingsFolder;
    }

    juce::File Amiga::getSettingsFile()
    {
        return getSettingsFolder().getChildFile("amigaMon.settings");
    }

    void Amiga::loadSettings()
    {
        auto settingsFile = getSettingsFile();
        auto settingsJson = juce::JSON::parse(settingsFile);
        if(settingsJson.isObject())
        {
            auto settings = settingsJson.getDynamicObject();
            displayHeight.store(settings->getProperty("height"));
            displayWidth.store(settings->getProperty("width"));
            displayOffsetX.store(settings->getProperty("xOffset"));
            displayOffsetY.store(settings->getProperty("yOffset"));
            audioMixFactor.store(settings->getProperty("audioMixFactor"));
        }
    }

    void Amiga::saveSettings()
    {
        std::map<juce::Identifier, juce::var> settings;
        settings["xOffset"] = displayOffsetX.load();
        settings["yOffset"] = displayOffsetY.load();
        settings["width"] = displayWidth.load();
        settings["height"] = displayHeight.load();
        settings["audioMixFactor"] = audioMixFactor.load();
        auto settingsJSON = juce::JSONUtils::makeObject(settings);
        auto settingsFile = getSettingsFile();
        settingsFile.replaceWithText(juce::JSON::toString(settingsJSON));
    }

    void Amiga::loadROM(const std::filesystem::path &path)
    {
        if(exists(path))
        {
            vAmiga.mem.loadRom(path);
        }
        if(!vAmiga.isLaunched())
            start();
    }
} // amigaMon namespace