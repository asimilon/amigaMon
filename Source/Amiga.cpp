//
// Created by Rich Elmes on 15.11.2024.
//

#include "Amiga.h"

#include <Emulator.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "Window.h"
#include "gui/MainComponent.h"
#include "gui/ControlsComponent.h"

namespace amigaMon {
    inline static Amiga* instance = nullptr;

    Amiga::Amiga()
    {
        instance = this;
        vAmiga.mem.loadRom(std::filesystem::current_path().parent_path() += std::filesystem::path("/roms/kick.rom"));
        vAmiga.emu->initialize();

        audioDeviceManager.addAudioCallback(this);
        audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);
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
        vAmiga.launch(this, &Amiga::callback);
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

    void Amiga::audioDeviceIOCallbackWithContext(const float * const *inputChannelData, int numInputChannels,
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
            constexpr auto factor = 0.3f;
            outputChannelData[leftChannel][index] = left + factor * right;
            outputChannelData[rightChannel][index] = right + factor * left;
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

    void Amiga::setStartDirectory(juce::File directory)
    {
        startDirectory = directory;
    }

    void Amiga::callback(const void *thisRef, Message message)
    {
        if(instance == nullptr)
            return;
        auto& vAmiga = instance->vAmiga;

        if(message.type == MSG_RSH_UPDATE)
        {
            vAmiga.emu->run();
        }
    }
} // amigaMon namespace