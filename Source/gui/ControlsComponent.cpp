//
// Created by Rich Elmes on 16.11.2024.
//

#include "ControlsComponent.h"

#include <Emulator.h>

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
        const auto alpha = !amiga.getAmiga().emu->isSuspended() && isMouseButtonDown()
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

    LoadDiskComponent::LoadDiskComponent(amigaMon::Amiga &amigaToUse)
        : IconBase(amigaToUse)
    {
        startTimerHz(30);
    }

    void LoadDiskComponent::paint(juce::Graphics &g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        drawBGandSetFont(g, alpha);
        const auto textHeight = 60 * ((float)getHeight() / 200.0f);
        const auto width = 200 * ((float)getWidth() / 200.0f);
        const auto bounds = getLocalBounds().toFloat().withSizeKeepingCentre(width, textHeight);
        g.drawText ("Load Disk", bounds, juce::Justification::centredTop, true);
        g.setColour(juce::Colours::black.withAlpha(juce::ModifierKeys::currentModifiers.isShiftDown() ? 1.0f : 0.1f));
        g.drawText ("& Reset", bounds, juce::Justification::centredBottom, true);
        g.setColour(juce::Colours::black);
        g.setFont (11.0f * ((float)getHeight() / 200.0f));
        g.drawText (filename, getLocalBounds().reduced(10, 10), juce::Justification::centredBottom, true);
    }

    void LoadDiskComponent::mouseUp(const juce::MouseEvent &event)
    {
        if(event.mods.isLeftButtonDown())
        {
            fileChooser = std::make_unique<juce::FileChooser> ("Select a disk image",
                                               amiga.getStartDirectory(), "*.adf");
            auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
            auto shiftDown = event.mods.isShiftDown();
            fileChooser->launchAsync (folderChooserFlags, [this, shiftDown] (const juce::FileChooser& chooser)
            {
                if(chooser.getResult().exists())
                {
                    juce::File diskFile (chooser.getResult());
                    filename = diskFile.getFileNameWithoutExtension();
                    amiga.setStartDirectory(diskFile.getParentDirectory());
                    amiga.loadDiskAndReboot(diskFile.getFullPathName().toStdString(), shiftDown);
                }
                else
                {
                    amiga.getAmiga().hardReset();
                }
            });
        }
    }

    void LoadDiskComponent::timerCallback()
    {
        repaint();
    }

    LoadRomComponent::LoadRomComponent(amigaMon::Amiga& amigaToUse)
        : IconBase(amigaToUse)
    {
    }

    void LoadRomComponent::paint(juce::Graphics& g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        drawBGandSetFont(g, alpha);
        g.drawText("Load ROM", getLocalBounds(), juce::Justification::centred, true);
        g.setFont (11.0f * ((float)getHeight() / 200.0f));
        g.drawText (amiga.getAmiga().mem.getRomTraits().title, getLocalBounds().reduced(10, 10), juce::Justification::centredBottom, true);
    }

    void LoadRomComponent::mouseUp(const juce::MouseEvent& event)
    {
        if(event.mods.isLeftButtonDown())
        {
            fileChooser = std::make_unique<juce::FileChooser> ("Select a ROM image",
                                                               amiga.getStartDirectory(), "*.rom");
            auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
            fileChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
            {
                if(chooser.getResult().exists())
                {
                    juce::File diskFile (chooser.getResult());
                    amiga.loadROM(diskFile.getFullPathName().toStdString());
                }
                else
                {
                    amiga.getAmiga().hardReset();
                }
            });
        }
    }

    ControlsComponent::ControlsComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        addAndMakeVisible(loadDiskComponent);
        addAndMakeVisible(loadRomComponent);
        addAndMakeVisible(playPauseComponent);
        addAndMakeVisible(stepFrameComponent);

        setSize (600, 200);
    }

    void ControlsComponent::resized()
    {
        auto bounds = getLocalBounds();

        loadDiskComponent.setBounds(bounds.removeFromLeft(getHeight()).reduced(1, 1));
        loadRomComponent.setBounds(bounds.removeFromLeft(getHeight()).reduced(1, 1));

        {
            auto fourway = bounds.removeFromLeft(getHeight());
            auto top = fourway.removeFromTop(fourway.getHeight() / 2);
            [[ maybe_unused ]] auto topLeft = top.removeFromLeft(top.getHeight()).reduced(1, 1);
            [[ maybe_unused ]] auto topRight = top.reduced(1, 1);
            [[ maybe_unused ]] auto bottomLeft = fourway.removeFromLeft(fourway.getHeight()).reduced(1, 1);
            [[ maybe_unused ]] auto bottomRight = fourway.reduced(1, 1);

            playPauseComponent.setBounds(topLeft);
            stepFrameComponent.setBounds(topRight);
        }
    }

    void ControlsComponent::paint(juce::Graphics& g)
    {
        g.fillAll (juce::Colours::black);
    }
} // amigaMon namespace