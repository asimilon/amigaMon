//
// Created by Rich Elmes on 16.11.2024.
//

#pragma once

#include "Amiga.h"

namespace amigaMon {
    class LoadDiskComponent final : public juce::Component, public juce::Timer
    {
    public:
        explicit LoadDiskComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;

        void mouseUp(const juce::MouseEvent& event) override;
        void timerCallback() override;
    private:
        [[ maybe_unused ]] amigaMon::Amiga& amiga;

        std::unique_ptr<juce::FileChooser> fileChooser;

        juce::String filename;
    };

    class PlayPauseComponent final : public juce::Component
    {
    public:
        explicit PlayPauseComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        [[ maybe_unused ]] amigaMon::Amiga& amiga;
    };

    class StepFrameComponent final : public juce::Component
    {
    public:
        explicit StepFrameComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        [[ maybe_unused ]] amigaMon::Amiga& amiga;
    };

    class ControlsComponent final : public juce::Component
    {
    public:
        explicit ControlsComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void resized() override;
    private:
        [[ maybe_unused ]] amigaMon::Amiga& amiga;

        LoadDiskComponent loadDiskComponent { amiga };
        PlayPauseComponent playPauseComponent { amiga };
        StepFrameComponent stepFrameComponent { amiga };
    };
} // amigaMon namespace