//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include "IconBase.h"
#include "../RecentFiles.h"

namespace amigaMon {
    class LoadRomComponent final : public IconBase
    {
    public:
        explicit LoadRomComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        std::unique_ptr<juce::FileChooser> fileChooser;

        RecentFiles recentFiles { amigaMon::Amiga::getSettingsFolder().getChildFile("rom.recents"), 5 };
    };
} // amigaMon namespace