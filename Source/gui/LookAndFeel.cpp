//
// Created by Rich Elmes on 23.11.2024.
//

#include "LookAndFeel.h"

namespace amigaMon {
    LookAndFeel::LookAndFeel()
    {
        setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::white);
        setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::white);
        setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);
        setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::black);
        setColour(juce::TextButton::ColourIds::textColourOnId + 1, juce::Colours::white);
    }

    void LookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
    {
        auto cornerSize = 0.0f;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        {
            constexpr auto amount = 0.2f;
            if (baseColour.getBrightness() > 0.5f)
                baseColour = baseColour.darker(amount);
            else
                baseColour = baseColour.brighter(amount);
        }

        g.setColour (baseColour);

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::TextButton::ColourIds::textColourOnId + 1));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }
        else
        {
            g.fillRoundedRectangle (bounds, cornerSize);

            g.setColour (button.findColour (juce::TextButton::ColourIds::textColourOnId + 1));
            g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
        }
    }
} // amigaMon namespace