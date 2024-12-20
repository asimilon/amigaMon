//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <VAmiga.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/LookAndFeel.h"

namespace amigaMon {
    class Amiga : public juce::AudioIODeviceCallback, public juce::ChangeBroadcaster
    {
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
        void audioDeviceError (const juce::String& /*errorMessage*/) override {}

        juce::File getStartDirectory();

        void advanceFrame();

        int getDisplayOffsetX() const;
        int getDisplayOffsetY() const;
        void setDisplayOffsetX(int newDisplayOffsetX);
        void setDisplayOffsetY(int newDisplayOffsetY);

        int getDisplayWidth() const;
        int getDisplayHeight() const;
        void setDisplayWidth(int newDisplayWidth);
        void setDisplayHeight(int newDisplayHeight);

        void setStartDirectory(juce::File directory);

        void showDisplaySettings();

        float getAudioMixFactor() const;
        void  setAudioMixFactor(float newAudioMixFactor);

        void showSaveSettingsPopup();

        void enableAutoQuit();

        void closeQuitConfirmation();

        juce::LookAndFeel* getLookAndFeel();

        static void callback(const void *thisRef, Message message);

        std::atomic<bool> shouldAdvanceFrame { false };
        std::atomic<bool> shouldPause { false };

        int getSizeMultiply() const;
        void setSizeMultiply(int newSizeMultiply);

        void saveSettings();

        static juce::File getSettingsFolder();
    private:
        void showQuitConfirmation();

        static constexpr double aspectRatio = 600.0 / 200.0;
        static juce::File getSettingsFile();

        void loadSettings();

        amigaMon::LookAndFeel lookAndFeel;
        vamiga::VAmiga vAmiga;

        juce::AudioDeviceManager audioDeviceManager;

        double sampleRate = 0.0;

        juce::File startDirectory = juce::File::getSpecialLocation(juce::File::userHomeDirectory);

        std::atomic<int> sizeMultiply { 2 };
        std::atomic<int> displayOffsetX { 76 };
        std::atomic<int> displayOffsetY { 29 };
        std::atomic<int> displayWidth { 354 };
        std::atomic<int> displayHeight { 283 };
        std::atomic<float> audioMixFactor { 0.5f };

        std::unique_ptr<juce::DocumentWindow> mainWindow;
        std::unique_ptr<juce::DocumentWindow> controlsWindow;
        std::unique_ptr<juce::DocumentWindow> screenSizeWindow;
        std::unique_ptr<juce::DocumentWindow> debugWindow;
        std::unique_ptr<juce::DocumentWindow> confirmQuitWindow;
        std::unique_ptr<juce::ComponentBoundsConstrainer> controlsWindowConstrainer;

        bool canAutoQuit = false;
    };
} // amigaMon namespace