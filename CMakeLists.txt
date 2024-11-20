cmake_minimum_required(VERSION 3.21)

include(CMake/CMakeCommon.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(productName "amigaMon")

project("${productName}" VERSION 0.0.3)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")
createProject("${productName}")

CPMAddPackage(
        NAME vAmiga
        GIT_REPOSITORY https://github.com/asimilon/vAmiga.git
        GIT_TAG origin/halt_check
        SOURCE_SUBDIR Emulator
)

set(SourceFolders
        Source
)

include(Source/SourceFiles.cmake)

set(JUCE_DEPENDENCIES
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_opengl
        vAmigaCore
)

set(JUCE_OPTIONS
        JUCE_APPLICATION_NAME_STRING="${productName}"
        JUCE_APPLICATION_VERSION_STRING="${PROJECT_VERSION}"
)

target_compile_definitions(vAmigaCore
        PUBLIC
            TPP=1
)

addGuiApp("${productName}" "${SourceFiles}" "${JUCE_DEPENDENCIES}" "${SourceFolders}" "Resources/icon.png" "${JUCE_OPTIONS}")

list(TRANSFORM SourceFolders PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
target_include_directories("${PROJECT_NAME}" PRIVATE "${SourceFolders}" "${CMAKE_CURRENT_SOURCE_DIR}")