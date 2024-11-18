//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once

#include "IconBase.h"

namespace amigaMon {
    class AudioMixComponent final : public IconBase
    {
    public:
        explicit AudioMixComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        juce::Path stereoIcon;

        juce::Point<float> lastMousePos;
    };
} // amigaMon namespace