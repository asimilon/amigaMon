//
// Created by Rich Elmes on 23.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace amigaMon {
    class LookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        LookAndFeel();

        void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    private:
    };
} // amigaMon namespace