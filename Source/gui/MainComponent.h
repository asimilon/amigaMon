//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>
#include "Amiga.h"

namespace amigaMon {
    class MainComponent final : public juce::Component, public juce::OpenGLRenderer, public juce::Timer
    {
    public:
        explicit MainComponent(amigaMon::Amiga& amigaToUse);
        ~MainComponent() override;

        void setTextureData(const uint32_t* buffer);

        void newOpenGLContextCreated() override;
        void openGLContextClosing() override;
        void renderOpenGL() override;

        void timerCallback() override;
    private:
        void uploadTexture();

        static constexpr int width = 320 + 24;
        static constexpr int height = 268;

        juce::OpenGLContext openGLContext;
        GLuint textureID = 0;
        const uint32_t* textureBuffer = nullptr;
        bool textureNeedsUpdate = false;

        amigaMon::Amiga& amiga;

        std::vector<uint32_t> glBuffer;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    };
} // amigaMon namespace