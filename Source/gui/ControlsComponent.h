//
// Created by Rich Elmes on 16.11.2024.
//

#pragma once

#include "Amiga.h"

namespace amigaMon {
    class IconBase : public juce::Component
    {
    public:
        explicit IconBase(amigaMon::Amiga& amigaToUse);
    protected:
        void drawBGandSetFont(juce::Graphics& g, float bgAlpha);

        [[ maybe_unused ]] amigaMon::Amiga& amiga;
    };

    class LoadDiskComponent final : public IconBase, public juce::Timer
    {
    public:
        explicit LoadDiskComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;

        void mouseUp(const juce::MouseEvent& event) override;
        void timerCallback() override;
    private:
        std::unique_ptr<juce::FileChooser> fileChooser;

        juce::String filename;
    };

    class PlayPauseComponent final : public IconBase
    {
    public:
        explicit PlayPauseComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
    };

    class LoadRomComponent final : public IconBase
    {
    public:
        explicit LoadRomComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        std::unique_ptr<juce::FileChooser> fileChooser;
    };

    class StepFrameComponent final : public IconBase, public juce::Timer
    {
    public:
        explicit StepFrameComponent(amigaMon::Amiga& amigaToUse);
        ~StepFrameComponent() override;

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
        void timerCallback() override;
    private:
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
        LoadRomComponent  loadRomComponent  { amiga };
        PlayPauseComponent playPauseComponent { amiga };
        StepFrameComponent stepFrameComponent { amiga };
    };
} // amigaMon namespace