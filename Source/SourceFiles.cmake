set(SourceFiles
        Main.cpp
        Amiga.cpp

        gui/MainComponent.cpp
        gui/ControlsComponent.cpp
        gui/ScreenSizeComponent.cpp
        gui/RecentFiles.cpp
        gui/DebugComponent.cpp
        gui/NativeTitleBarHeight.mm
        gui/CanQuitComponent.cpp
        gui/LookAndFeel.cpp

        gui/controls/AudioMixComponent.cpp
        gui/controls/IconBase.cpp
        gui/controls/LoadDiskComponent.cpp
        gui/controls/LoadRomComponent.cpp
        gui/controls/PlayPauseStepComponents.cpp
        gui/controls/SizeComponent.cpp
)

list(TRANSFORM SourceFiles PREPEND Source/)