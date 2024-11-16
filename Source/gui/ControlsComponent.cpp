//
// Created by Rich Elmes on 16.11.2024.
//

#include "ControlsComponent.h"

#include <Emulator.h>

namespace amigaMon {
    StepFrameComponent::StepFrameComponent(amigaMon::Amiga &amigaToUse)
        : amiga(amigaToUse)
    {
        setRepaintsOnMouseActivity(true);
        setSize(200, 200);
    }

    void StepFrameComponent::paint(juce::Graphics &g)
    {
        const auto alpha = isMouseButtonDown()
                         ? 0.7f
                         : (!amiga.getAmiga().emu->isSuspended() && isMouseOver() ? 0.6f : 0.5f);
        g.fillAll (juce::Colours::white.withAlpha(alpha));
        g.setColour (juce::Colours::black.withAlpha(amiga.getAmiga().emu->isSuspended() ? 0.6f : 1.0f));
        g.setFont (22.0f * ((float)getHeight() / 200.0f));
        g.drawText ("Step 1 Frame", getLocalBounds().toFloat(), juce::Justification::centred, true);
    }

    void StepFrameComponent::mouseUp(juce::MouseEvent const &event)
    {
        if(amiga.getAmiga().emu->isSuspended() && event.mods.isLeftButtonDown())
        {
            amiga.advanceFrame();
        }
    }

    PlayPauseComponent::PlayPauseComponent(amigaMon::Amiga &amigaToUse)
        : amiga(amigaToUse)
    {
        setRepaintsOnMouseActivity(true);
        setSize(200, 200);
    }

    void PlayPauseComponent::paint(juce::Graphics& g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        g.fillAll (juce::Colours::white.withAlpha(alpha));
        g.setColour (juce::Colours::black);
        g.setFont (22.0f * ((float)getHeight() / 200.0f));
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
        : amiga(amigaToUse)
    {
        setRepaintsOnMouseActivity(true);
        setSize(200, 200);
        startTimerHz(30);
    }

    void LoadDiskComponent::paint(juce::Graphics &g)
    {
        const auto alpha = isMouseButtonDown() ? 0.7f : (isMouseOver() ? 0.6f : 0.5f);
        g.fillAll (juce::Colours::white.withAlpha(alpha));
        g.setColour (juce::Colours::black);
        g.setFont (22.0f * ((float)getHeight() / 200.0f));
        const auto textHeight = 60 * ((float)getHeight() / 200.0f);
        const auto width = 200 * ((float)getWidth() / 200.0f);
        const auto bounds = getLocalBounds().toFloat().withSizeKeepingCentre(width, textHeight);
        g.drawText ("Load Disk", bounds, juce::Justification::centredTop, true);
        if(juce::ModifierKeys::currentModifiers.isShiftDown())
        {
            g.drawText ("& Reset", bounds, juce::Justification::centredBottom, true);
        }
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

    ControlsComponent::ControlsComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        addAndMakeVisible(loadDiskComponent);
        addAndMakeVisible(playPauseComponent);
        addAndMakeVisible(stepFrameComponent);

        setSize (600, 200);
    }

    void ControlsComponent::resized()
    {
        auto bounds = getLocalBounds();

        loadDiskComponent.setBounds(bounds.removeFromLeft(getHeight()).reduced(1, 1));

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