//
// Created by Rich Elmes on 18.11.2024.
//

#include "LoadDiskComponent.h"

namespace amigaMon {

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
                    recentFiles.addFile(diskFile);
                }
                else
                {
                    amiga.getAmiga().hardReset();
                }
            });
        }
        else if (event.mods.isPopupMenu())
        {
            juce::PopupMenu menu;
            auto recents = recentFiles.getRecentFiles();
            const auto shiftDown = event.mods.isShiftDown();
            for (const auto& file : recents)
            {
                menu.addItem(file.getFileName(), [this, file, shiftDown] {
                    filename = file.getFileNameWithoutExtension();
                    amiga.loadDiskAndReboot(file.getFullPathName().toStdString(), shiftDown);
                });
            }
            menu.showMenuAsync(juce::PopupMenu::Options(), [](int){});
        }
    }

    void LoadDiskComponent::timerCallback()
    {
        repaint();
    }

} // amigaMon namespace