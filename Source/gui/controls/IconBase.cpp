//
// Created by Rich Elmes on 18.11.2024.
//

#include "IconBase.h"

namespace amigaMon {
    IconBase::IconBase(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        setRepaintsOnMouseActivity(true);
        setSize(200, 200);
    }

    void IconBase::drawBGandSetFont(juce::Graphics& g, float bgAlpha)
    {
        g.fillAll (juce::Colours::white.withAlpha(bgAlpha));
        g.setFont (22.0f * ((float)getHeight() / 200.0f));
        g.setColour (juce::Colours::black);
    }
} // amigaMon namespace