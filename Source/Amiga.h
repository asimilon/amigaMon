//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <VAmiga.h>
#include <juce_graphics/juce_graphics.h>

namespace amigaMon {
    class Amiga {
    public:
        Amiga();

        void start();

        vamiga::VAmiga& getAmiga();

    private:
        static void callback(const void *thisRef, Message message);

        vamiga::VAmiga vAmiga;
    };
} // amigaMon namespace