//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <Emulator.h>
#include "Amiga.h"

namespace amigaMon {
    class IconBase : public juce::Component
    {
    public:
        explicit IconBase(amigaMon::Amiga& amigaToUse);
    protected:
        void drawBGandSetFont(juce::Graphics& g, float bgAlpha);

        amigaMon::Amiga& amiga;
    };
} // amigaMon namespace