//
// Created by Rich Elmes on 18.11.2024.
//

#include "RecentFiles.h"

#include <utility>

namespace amigaMon {
    RecentFiles::RecentFiles(juce::File recentsFileToUse, int size)
        : recentsFile(std::move(recentsFileToUse))
        , recentFilesMaxSize(size)
    {
        // read the file paths from the file, one per line
        juce::FileInputStream inputStream(recentsFile);
        if (inputStream.openedOk())
        {
            juce::String line;
            while (!inputStream.isExhausted())
            {
                line = inputStream.readNextLine();
                if (line.isNotEmpty()) recentFiles.emplace_back(line);
            }
        }
    }

    RecentFiles::~RecentFiles()
    {
        // save the recent files as strings, one per line
        juce::TemporaryFile tempFile(recentsFile);
        juce::FileOutputStream outputStream(tempFile.getFile());
        if (outputStream.openedOk())
        {
            for (const auto& file : recentFiles)
            {
                outputStream << file.getFullPathName() << "\n";
            }
        }
        tempFile.overwriteTargetFileWithTemporary();
    }

    std::vector<juce::File> RecentFiles::getRecentFiles() const
    {
        return { recentFiles.begin(), recentFiles.end() };
    }

    void RecentFiles::addFile(juce::File file)
    {
        // if the file is already in the list move it to the front
        auto it = std::find(recentFiles.begin(), recentFiles.end(), file);
        if (it != recentFiles.end())
        {
            recentFiles.erase(it);
        }
        recentFiles.push_front(file);
        while (recentFiles.size() > recentFilesMaxSize)
        {
            recentFiles.pop_back();
        }
    }
} // amigaMon namespace