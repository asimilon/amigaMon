//
// Created by Rich Elmes on 18.11.2024.
//

#include "SizeComponent.h"

namespace amigaMon {

    SizeComponent::SizeComponent(amigaMon::Amiga& amigaToUse)
        : IconBase(amigaToUse)
    {
    }

    void SizeComponent::paint(juce::Graphics& g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        drawBGandSetFont(g, alpha);
        g.drawText("Size: " + juce::String(amiga.getSizeMultiply()) + "x", getLocalBounds(), juce::Justification::centred, true);
    }

    void SizeComponent::mouseUp(juce::MouseEvent const &event)
    {
        if(event.mods.isLeftButtonDown())
        {
            juce::PopupMenu menu;
            menu.addItem("1x", [this]() { amiga.setSizeMultiply(1); });
            menu.addItem("2x", [this]() { amiga.setSizeMultiply(2); });
            menu.addItem("3x", [this]() { amiga.setSizeMultiply(3); });
            menu.addItem("4x", [this]() { amiga.setSizeMultiply(4); });
            menu.addItem("5x", [this]() { amiga.setSizeMultiply(5); });
            menu.addSeparator();
            menu.addItem("Settings...", [this]() { amiga.showDisplaySettings(); });
            menu.showMenuAsync(juce::PopupMenu::Options(), [this](int){ repaint(); });
        }
    }

} // amigaMon namespace