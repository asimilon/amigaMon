//
// Created by Rich Elmes on 16.11.2024.
//

#include "ControlsComponent.h"

#include <Emulator.h>

namespace amigaMon {
    ControlsComponent::ControlsComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        addAndMakeVisible(loadDiskComponent);
        addAndMakeVisible(loadRomComponent);
        addAndMakeVisible(playPauseComponent);
        addAndMakeVisible(stepFrameComponent);
        addAndMakeVisible(sizeComponent);
        addAndMakeVisible(audioMixComponent);

        setSize (600, 200);
    }

    void ControlsComponent::resized()
    {
        auto bounds = getLocalBounds();

        loadDiskComponent.setBounds(bounds.removeFromLeft(getHeight()).reduced(1, 1));
        loadRomComponent.setBounds(bounds.removeFromLeft(getHeight()).reduced(1, 1));

        {
            auto fourway = bounds.removeFromLeft(getHeight());
            auto top = fourway.removeFromTop(fourway.getHeight() / 2);
            auto topLeft = top.removeFromLeft(top.getHeight()).reduced(1, 1);
            auto topRight = top.reduced(1, 1);
            auto bottomLeft = fourway.removeFromLeft(fourway.getHeight()).reduced(1, 1);
            auto bottomRight = fourway.reduced(1, 1);

            playPauseComponent.setBounds(topLeft);
            stepFrameComponent.setBounds(topRight);
            sizeComponent.setBounds(bottomLeft);
        }

        auto lastSection = bounds.removeFromLeft(getHeight());
        audioMixComponent.setBounds(lastSection.removeFromLeft(lastSection.getWidth() * 0.4f).reduced(1, 1));
    }

    void ControlsComponent::paint(juce::Graphics& g)
    {
        g.fillAll (juce::Colours::black);
    }
} // amigaMon namespace