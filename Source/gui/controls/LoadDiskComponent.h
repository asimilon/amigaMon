//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include "IconBase.h"
#include "../RecentFiles.h"

namespace amigaMon {
    class LoadDiskComponent final : public IconBase, public juce::Timer
    {
    public:
        explicit LoadDiskComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;

        void mouseUp(const juce::MouseEvent& event) override;
        void timerCallback() override;
    private:
        std::unique_ptr<juce::FileChooser> fileChooser;

        juce::String filename;

        RecentFiles recentFiles { amigaMon::Amiga::getSettingsFolder().getChildFile("disk.recents"), 10 };
    };

} // amigaMon namespace