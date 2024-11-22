//
// Created by Rich Elmes on 18.11.2024.
//

#pragma once
#include <juce_core/juce_core.h>

namespace amigaMon {
    class RecentFiles {
    public:
        RecentFiles(juce::File recentsFileToUse, size_t size);
        ~RecentFiles();

        [[nodiscard]] std::vector<juce::File> getRecentFiles() const;

        void addFile(juce::File file);
    private:
        void saveRecents();

        const juce::File recentsFile;
        const size_t recentFilesMaxSize;

        std::list<juce::File> recentFiles;
    };
} // amigaMon namespace