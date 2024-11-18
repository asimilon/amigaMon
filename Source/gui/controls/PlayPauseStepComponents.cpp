//
// Created by Rich Elmes on 18.11.2024.
//

#include "PlayPauseStepComponents.h"

namespace amigaMon {

    PlayPauseComponent::PlayPauseComponent(amigaMon::Amiga &amigaToUse)
        : IconBase(amigaToUse)
    {
    }

    void PlayPauseComponent::paint(juce::Graphics& g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        drawBGandSetFont(g, alpha);
        g.drawText (juce::String(amiga.getAmiga().emu->isSuspended() ? "Play" : "Pause"), getLocalBounds().toFloat(), juce::Justification::centred, true);
    }

    void PlayPauseComponent::mouseUp(const juce::MouseEvent &event)
    {
        if(event.mods.isLeftButtonDown())
        {
            if(amiga.getAmiga().emu->isSuspended())
                amiga.getAmiga().emu->resume();
            else
                amiga.getAmiga().emu->suspend();
            repaint();
        }
    }

    /**********************/
    /* StepFrameComponent */
    /**********************/

    StepFrameComponent::StepFrameComponent(amigaMon::Amiga &amigaToUse)
        : IconBase(amigaToUse)
    {
        startTimerHz(10);
    }

    StepFrameComponent::~StepFrameComponent()
    {
        stopTimer();
    }

    void StepFrameComponent::paint(juce::Graphics &g)
    {
        const auto alpha =  amiga.getAmiga().emu->isSuspended() && isMouseButtonDown()
                         ? 0.7f
                         : (amiga.getAmiga().emu->isSuspended() && isMouseOver() ? 0.6f : 0.5f);
        drawBGandSetFont(g, alpha);
        g.setColour (juce::Colours::black.withAlpha(!amiga.getAmiga().emu->isSuspended() ? 0.6f : 1.0f));
        g.drawText ("Step 1 Frame", getLocalBounds().toFloat(), juce::Justification::centred, true);
    }

    void StepFrameComponent::mouseUp(juce::MouseEvent const &event)
    {
        if(amiga.getAmiga().emu->isSuspended() && event.mods.isLeftButtonDown())
        {
            amiga.advanceFrame();
        }
    }

    void StepFrameComponent::timerCallback()
    {
        repaint();
    }

} // amigaMon namespace