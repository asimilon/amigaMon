set(SourceFiles
        Main.cpp
        Amiga.cpp

        gui/MainComponent.cpp
        gui/ControlsComponent.cpp
        gui/ScreenSizeComponent.cpp
)

list(TRANSFORM SourceFiles PREPEND Source/)