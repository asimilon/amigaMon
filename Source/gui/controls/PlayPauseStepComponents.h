//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include "IconBase.h"

namespace amigaMon {
    class PlayPauseComponent final : public IconBase
    {
    public:
        explicit PlayPauseComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
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
} // amigaMon namespace