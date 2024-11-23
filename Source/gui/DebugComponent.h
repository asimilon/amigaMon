//
// Created by Rich Elmes on 23.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Amiga.h"

namespace amigaMon {
class DebugComponent final : public juce::Component
{
public:
    explicit DebugComponent(amigaMon::Amiga& amigaToUse);

    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    [[ maybe_unused ]] amigaMon::Amiga& amiga;
};
} // amigaMon namespace