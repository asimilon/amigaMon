//
// Created by Rich Elmes on 23.11.2024.
//

#include "DebugComponent.h"

namespace amigaMon {
    DebugComponent::DebugComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        setSize(350, 600);
        toFront(false);
    }

    void DebugComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::black);
    }

    void DebugComponent::resized()
    {
    }
} // amigaMon namespace