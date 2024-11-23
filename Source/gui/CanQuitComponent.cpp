//
// Created by Rich Elmes on 23.11.2024.
//

#include "CanQuitComponent.h"

namespace amigaMon {
    CanQuitComponent::CanQuitComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        setLookAndFeel(amiga.getLookAndFeel());
        addAndMakeVisible(quitButton);
        quitButton.onClick = [this]() {
            amiga.enableAutoQuit();
            juce::JUCEApplicationBase::quit();
        };
        addAndMakeVisible(cancelButton);
        cancelButton.onClick = [this]() {
            amiga.closeQuitConfirmation();
        };
        setSize(300, 200);
    }

    CanQuitComponent::~CanQuitComponent()
    {
        setLookAndFeel(nullptr);
    }

    void CanQuitComponent::resized()
    {
        constexpr auto buttonWidth = 100;
        constexpr auto buttonHeight = 30;
        constexpr auto buttonGap = 20;
        constexpr auto totalWidth = buttonWidth * 2 + buttonGap;
        auto bounds = getLocalBounds().withSizeKeepingCentre(totalWidth, buttonHeight);
        quitButton.setBounds(bounds.removeFromLeft(buttonWidth));
        bounds.removeFromLeft(buttonGap);
        cancelButton.setBounds(bounds.removeFromLeft(buttonWidth));
    }

    void CanQuitComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(0xff121212));
    }
} // amigaMon namespace