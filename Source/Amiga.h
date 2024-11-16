//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <VAmiga.h>
#include <juce_audio_devices/juce_audio_devices.h>

namespace amigaMon {
    class Amiga : public juce::AudioIODeviceCallback {
    public:
        Amiga();
        ~Amiga() override;

        void start();

        vamiga::VAmiga& getAmiga();

        void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
        void audioDeviceStopped() override;
        void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                              float* const* outputChannelData, int numOutputChannels,
                                              int numSamples, const juce::AudioIODeviceCallbackContext& context) override;
        void audioDeviceError (const juce::String& errorMessage) override {};
    private:
        static void callback(const void *thisRef, Message message);

        vamiga::VAmiga vAmiga;

        juce::AudioDeviceManager audioDeviceManager;

        double sampleRate = 0.0;
    };
} // amigaMon namespace