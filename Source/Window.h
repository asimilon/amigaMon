//
// Created by Rich Elmes on 16.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Amiga.h"

namespace amigaMon {
    template <typename T>
class Window final : public juce::DocumentWindow
    {
    public:
        explicit Window (juce::String name, amigaMon::Amiga& amiga, bool isResizable = false, juce::ComponentBoundsConstrainer* constrainerToUse = nullptr)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (ResizableWindow::backgroundColourId),
                              DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new T(amiga), true);

            setResizable (isResizable, isResizable);
            centreWithSize (getWidth(), getHeight());

            if(constrainerToUse != nullptr)
            {
                setConstrainer(constrainerToUse);
            }

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            onCloseButtonPressed();
        }

        static constexpr bool resizable = true;

        std::function<void()> onCloseButtonPressed = []() {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        };
    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Window)
    };
}; // amigaMon namespace