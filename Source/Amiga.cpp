//
// Created by Rich Elmes on 15.11.2024.
//

#include "Amiga.h"

#include <Emulator.h>
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <thread>

#include <utility>

namespace amigaMon {
    Amiga::Amiga()
    {
        vAmiga.mem.loadRom(std::filesystem::current_path().parent_path() += std::filesystem::path("/roms/kick.rom"));
        auto disk = vamiga::FloppyFile::make(std::filesystem::current_path().parent_path() += std::filesystem::path("/disks/df0.adf"));
        vAmiga.df0.insertMedia(*disk, true);
        vAmiga.emu->initialize();

        audioDeviceManager.addAudioCallback(this);
        audioDeviceManager.initialise (0, 2, nullptr, true, {}, nullptr);
    }

    Amiga::~Amiga()
    {
        audioDeviceManager.removeAudioCallback(this);
    }

    void Amiga::start()
    {
        vAmiga.launch(this, &Amiga::callback);
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

    void Amiga::callback(const void *thisRef, Message message)
    {
        if(thisRef == nullptr)
            return;
        auto* amiga = static_cast<const amigaMon::Amiga*>(thisRef);
        auto& vAmiga = amiga->vAmiga;

        if(message.type == MSG_RSH_UPDATE)
        {
            vAmiga.emu->run();
        }
    }
} // amigaMon namespace