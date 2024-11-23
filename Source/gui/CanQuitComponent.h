//
// Created by Rich Elmes on 23.11.2024.
//

#pragma once

#include "Amiga.h"

namespace amigaMon {
    class CanQuitComponent : public juce::Component
{
    public:
        explicit CanQuitComponent(amigaMon::Amiga& amigaToUse);
        ~CanQuitComponent() override;

        void resized() override;
        void paint(juce::Graphics& g) override;
    private:
        amigaMon::Amiga& amiga;

        juce::TextButton quitButton{"Quit"};
        juce::TextButton cancelButton{"Cancel"};
    };
} // amigaMon namespace