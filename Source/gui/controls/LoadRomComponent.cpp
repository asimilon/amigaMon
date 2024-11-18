//
// Created by Rich Elmes on 18.11.2024.
//

#include "LoadRomComponent.h"

namespace amigaMon {

    LoadRomComponent::LoadRomComponent(amigaMon::Amiga& amigaToUse)
        : IconBase(amigaToUse)
    {
        auto recents = recentFiles.getRecentFiles();
        if (!recents.empty())
        {
            amiga.loadROM(recents.front().getFullPathName().toStdString());
        }
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
        if (event.mods.isLeftButtonDown())
        {
            fileChooser = std::make_unique<juce::FileChooser> ("Select a ROM image",
                                                               amiga.getStartDirectory(), "*.rom");
            auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
            fileChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
            {
                if (chooser.getResult().exists())
                {
                    juce::File diskFile (chooser.getResult());
                    amiga.loadROM(diskFile.getFullPathName().toStdString());
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
            for (const auto& file : recents)
            {
                menu.addItem(file.getFileName(), [this, file]() {
                    amiga.loadROM(file.getFullPathName().toStdString());
                });
            }
            menu.showMenuAsync(juce::PopupMenu::Options(), [](int){});
        }
    }

} // amigaMon namespace