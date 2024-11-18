//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include "IconBase.h"

namespace amigaMon {

    class SizeComponent final : public IconBase
    {
    public:
        explicit SizeComponent(amigaMon::Amiga& amigaToUse);

        void paint(juce::Graphics& g) override;
        void mouseUp(const juce::MouseEvent& event) override;
    };

} // amigaMon namespace