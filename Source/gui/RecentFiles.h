//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include <juce_core/juce_core.h>

namespace amigaMon {
    class RecentFiles {
    public:
        RecentFiles(juce::File recentsFileToUse, int size);
        ~RecentFiles();

        [[nodiscard]] std::vector<juce::File> getRecentFiles() const;

        void addFile(juce::File file);
    private:
        const juce::File recentsFile;
        const int recentFilesMaxSize;

        std::list<juce::File> recentFiles;
    };
} // amigaMon namespace