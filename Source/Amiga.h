//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <VAmiga.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace amigaMon {
    class Amiga : public juce::AudioIODeviceCallback {
    public:
        Amiga();
        ~Amiga() override;

        void initialiseGUI(juce::String name, amigaMon::Amiga& amiga);
        void shutdownGUI();
        bool canQuit();

        void start();

        void loadROM(const std::filesystem::path& path);
        void loadDiskAndReboot(const std::string& path, bool reboot);

        vamiga::VAmiga& getAmiga();

        void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
        void audioDeviceStopped() override;
        void audioDeviceIOCallbackWithContext(const float* const* inputChannelData, int numInputChannels,
                                              float* const* outputChannelData, int numOutputChannels,
                                              int numSamples, const juce::AudioIODeviceCallbackContext& context) override;
        void audioDeviceError (const juce::String& errorMessage) override {}

        juce::File getStartDirectory();

        void advanceFrame();;
        void setStartDirectory(juce::File directory);

        static void callback(const void *thisRef, Message message);

        std::atomic<bool> shouldAdvanceFrame { false };
        std::atomic<bool> shouldPause { false };

    private:

        static constexpr double aspectRatio = 600.0 / 200.0;

        vamiga::VAmiga vAmiga;

        juce::AudioDeviceManager audioDeviceManager;

        double sampleRate = 0.0;

        juce::File startDirectory = juce::File::getSpecialLocation(juce::File::userHomeDirectory);

        std::unique_ptr<juce::DocumentWindow> mainWindow;
        std::unique_ptr<juce::DocumentWindow> controlsWindow;
        std::unique_ptr<juce::ComponentBoundsConstrainer> controlsWindowConstrainer;
    };
} // amigaMon namespace