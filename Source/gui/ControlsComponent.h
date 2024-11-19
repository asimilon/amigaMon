//
// Created by Rich Elmes on 16.11.2024.
//

#pragma once

#include "Amiga.h"
#include "controls/AudioMixComponent.h"
#include "controls/LoadDiskComponent.h"
#include "controls/LoadRomComponent.h"
#include "controls/PlayPauseStepComponents.h"
#include "controls/SizeComponent.h"

namespace amigaMon {
    class ControlsComponent final : public juce::Component
    {
    public:
        explicit ControlsComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void resized() override;
    private:
        amigaMon::Amiga& amiga;

        LoadDiskComponent   loadDiskComponent   { amiga };
        LoadRomComponent    loadRomComponent    { amiga };
        PlayPauseComponent  playPauseComponent  { amiga };
        StepFrameComponent  stepFrameComponent  { amiga };
        SizeComponent       sizeComponent       { amiga };
        AudioMixComponent   audioMixComponent   { amiga };
    };
} // amigaMon namespace