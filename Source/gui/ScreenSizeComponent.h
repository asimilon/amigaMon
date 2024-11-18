//
// Created by Rich Elmes on 17.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Amiga.h"

namespace amigaMon {
    class ScreenSizeComponent : public juce::Component, public juce::Timer
    {
    public:
        explicit ScreenSizeComponent(amigaMon::Amiga& amigaToUse);
        ~ScreenSizeComponent() override;

        void paint(juce::Graphics& g) override;
        void timerCallback() override;

        void mouseMove(const juce::MouseEvent& event) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent &event) override;
        void mouseUp(const juce::MouseEvent &event) override;
    private:
        enum class MouseState
        {
            none,
            resizeLeft,
            resizeRight,
            resizeTop,
            resizeBottom
        };

        void showLabels(const MouseState& stateToCheck, juce::Graphics& g,
                        const juce::Rectangle<int>& bounds, const juce::Point<float>& centre);

        [[ maybe_unused ]] amigaMon::Amiga& amiga;

        MouseState mouseState = MouseState::none;
        MouseState currentDrag = MouseState::none;
        juce::Point<float> lastMousePos;
        float cumulativeDelta = 0.0f;

        juce::Image image;
    };
} // amigaMon namespace