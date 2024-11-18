//
// Created by Rich Elmes on 17.11.2024.
//

#include "ScreenSizeComponent.h"
#include "MainComponent.h"
#include <Emulator.h>

namespace amigaMon {
    ScreenSizeComponent::ScreenSizeComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        startTimerHz(1);
        setSize(MainComponent::textureWidth / 2, MainComponent::textureHeight);
    }

    ScreenSizeComponent::~ScreenSizeComponent()
    {
        stopTimer();
    }

    void ScreenSizeComponent::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
        g.drawImage(image, 0, 0, getWidth(), getHeight(), 0, 0, MainComponent::textureWidth, MainComponent::textureHeight);
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillRect(getLocalBounds());

        auto bounds = getLocalBounds();
        bounds.removeFromLeft(amiga.getDisplayOffsetX());
        bounds.removeFromTop(amiga.getDisplayOffsetY());
        bounds.setSize(amiga.getDisplayWidth(), amiga.getDisplayHeight());

        juce::Path path;
        path.addRoundedRectangle(bounds.toFloat(), 10.0f);
        {
            juce::Graphics::ScopedSaveState state(g);
            g.reduceClipRegion(path);
            g.drawImage(image, 0, 0, getWidth(), getHeight(), 0, 0, MainComponent::textureWidth, MainComponent::textureHeight);
        }

        g.setColour(juce::Colours::white);
        g.strokePath(path, juce::PathStrokeType(1.5f));

        auto centre = path.getBounds().getCentre();
        g.drawLine(0.0f, centre.y, bounds.getX(), centre.y);
        g.drawLine(bounds.getRight(), centre.y, getWidth(), centre.y);
        g.drawLine(centre.x, 0.0f, centre.x, bounds.getY());
        g.drawLine(centre.x, bounds.getBottom(), centre.x, getHeight());
        showLabels(currentDrag, g, bounds, centre);
        showLabels(mouseState, g, bounds, centre);
    }

    void ScreenSizeComponent::showLabels(const MouseState& stateToCheck, juce::Graphics& g,
                                         const juce::Rectangle<int>& bounds, const juce::Point<float>& centre)
    {
        switch(stateToCheck)
        {
            case MouseState::resizeLeft:
            {
                juce::Rectangle<float> r { (float)bounds.getX() + 1, centre.y - 10, 50, 20 };
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                g.fillRoundedRectangle(r, 2.0f);
                g.setColour(juce::Colours::black);
                g.drawText(juce::String(amiga.getDisplayOffsetX()), r, juce::Justification::centred);
                break;
            }
            case MouseState::resizeRight:
            {
                juce::Rectangle<float> r { (float)bounds.getRight() - 51, centre.y - 10, 50, 20 };
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                g.fillRoundedRectangle(r, 2.0f);
                g.setColour(juce::Colours::black);
                g.drawText(juce::String(amiga.getDisplayWidth()), r, juce::Justification::centred);
                break;
            }
            case MouseState::resizeTop:
            {
                juce::Rectangle<float> r { centre.x - 25, (float)bounds.getY() + 1, 50, 20 };
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                g.fillRoundedRectangle(r, 2.0f);
                g.setColour(juce::Colours::black);
                g.drawText(juce::String(amiga.getDisplayOffsetY()), r, juce::Justification::centred);
                break;
            }
            case MouseState::resizeBottom:
            {
                juce::Rectangle<float> r { centre.x - 25, (float)bounds.getBottom() - 21, 50, 20 };
                g.setColour(juce::Colours::white.withAlpha(0.6f));
                g.fillRoundedRectangle(r, 2.0f);
                g.setColour(juce::Colours::black);
                g.drawText(juce::String(amiga.getDisplayHeight()), r, juce::Justification::centred);
                break;
            }
            case MouseState::none:
                break;
        }
    }

    void ScreenSizeComponent::timerCallback()
    {
        auto pixels = amiga.getAmiga().emu->getTexture().pixels.ptr;
        // make a juce::Image from the pixels
        image = juce::Image(juce::Image::ARGB, MainComponent::textureWidth, MainComponent::textureHeight, true);
        juce::Image::BitmapData bitmapData(image, juce::Image::BitmapData::writeOnly);

        for (int y = 0; y < MainComponent::textureHeight; ++y) {
            for (int x = 0; x < MainComponent::textureWidth; ++x) {
                auto pixel = pixels[y * MainComponent::textureWidth + x];
                // rearrange the pixel order to RGBA
                const auto alpha = (pixel >> 24) & 0xFF;
                const auto red = (pixel >> 16) & 0xFF;
                const auto green = (pixel >> 8) & 0xFF;
                const auto blue = pixel & 0xFF;
                auto ptr = bitmapData.getPixelPointer(x, y);
                ptr[0] = red;
                ptr[1] = green;
                ptr[2] = blue;
                ptr[3] = alpha;
            }
        }
        repaint();
    }

    void ScreenSizeComponent::mouseMove(const juce::MouseEvent &event)
    {
        constexpr auto gap = 5;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        mouseState = MouseState::none;
        if(event.y >= amiga.getDisplayOffsetY() && event.y <= amiga.getDisplayOffsetY() + amiga.getDisplayHeight())
        {
            if(event.x > amiga.getDisplayOffsetX() - gap && event.x < amiga.getDisplayOffsetX() + gap)
            {
                setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
                mouseState = MouseState::resizeLeft;
            }
            else if(event.x > amiga.getDisplayOffsetX() + amiga.getDisplayWidth() - gap && event.x < amiga.getDisplayOffsetX() + amiga.getDisplayWidth() + gap)
            {
                setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
                mouseState = MouseState::resizeRight;
            }
        }
        else if(event.x >= amiga.getDisplayOffsetX() && event.x <= amiga.getDisplayOffsetX() + amiga.getDisplayWidth())
        {
            if(event.y > amiga.getDisplayOffsetY() - gap && event.y < amiga.getDisplayOffsetY() + gap)
            {
                setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
                mouseState = MouseState::resizeTop;
            }
            else if(event.y > amiga.getDisplayOffsetY() + amiga.getDisplayHeight() - gap && event.y < amiga.getDisplayOffsetY() + amiga.getDisplayHeight() + gap)
            {
                setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
                mouseState = MouseState::resizeBottom;
            }
        }
    }

    void ScreenSizeComponent::mouseDown(const juce::MouseEvent &event)
    {
        currentDrag = mouseState;
        lastMousePos = event.position;
        cumulativeDelta = 0.0f;
    }

    void ScreenSizeComponent::mouseDrag(const juce::MouseEvent &event)
    {
        if(currentDrag == MouseState::resizeLeft || currentDrag == MouseState::resizeRight)
        {
            auto deltaX = event.position.x - lastMousePos.x;
            cumulativeDelta += deltaX;
            auto delta = 0;
            if(cumulativeDelta > 1.0f)
            {
                while(cumulativeDelta > 1.0f)
                {
                    delta++;
                    cumulativeDelta -= 1.0f;
                }
            }
            else if(cumulativeDelta < -1.0f)
            {
                while(cumulativeDelta < -1.0f)
                {
                    delta--;
                    cumulativeDelta += 1.0f;
                }
            }
            if(currentDrag == MouseState::resizeLeft)
            {
                amiga.setDisplayOffsetX(amiga.getDisplayOffsetX() + delta);
                repaint();
            }
            else if(currentDrag == MouseState::resizeRight)
            {
                amiga.setDisplayWidth(amiga.getDisplayWidth() + delta);
                repaint();
            }
        }
        else if(currentDrag == MouseState::resizeTop || currentDrag == MouseState::resizeBottom)
        {
            auto deltaY = event.position.y - lastMousePos.y;
            cumulativeDelta += deltaY;
            auto delta = 0;
            if(cumulativeDelta > 1.0f)
            {
                while(cumulativeDelta > 1.0f)
                {
                    delta++;
                    cumulativeDelta -= 1.0f;
                }
            }
            else if(cumulativeDelta < -1.0f)
            {
                while(cumulativeDelta < -1.0f)
                {
                    delta--;
                    cumulativeDelta += 1.0f;
                }
            }
            if(currentDrag == MouseState::resizeTop)
            {
                amiga.setDisplayOffsetY(amiga.getDisplayOffsetY() + delta);
                repaint();
            }
            else if(currentDrag == MouseState::resizeBottom)
            {
                amiga.setDisplayHeight(amiga.getDisplayHeight() + delta);
                repaint();
            }
        }
        lastMousePos = event.position;
    }

    void ScreenSizeComponent::mouseUp(const juce::MouseEvent &event)
    {
        if(event.mods.isPopupMenu())
        {
            amiga.showSaveSettingsPopup();
        }
        currentDrag = MouseState::none;
        repaint();
    }
} // amigaMon namespace