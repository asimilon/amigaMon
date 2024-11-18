//
// Created by Rich Elmes on 15.11.2024.
//

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>
#include "Amiga.h"

namespace amigaMon {
    class MainComponent final : public juce::Component, public juce::OpenGLRenderer, public juce::ChangeListener
    {
    public:
        explicit MainComponent(amigaMon::Amiga& amigaToUse);
        ~MainComponent() override;

        void setTextureData(const uint32_t* buffer);

        void newOpenGLContextCreated() override;
        void openGLContextClosing() override;
        void renderOpenGL() override;

        void mouseUp(const juce::MouseEvent& event) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseMove(const juce::MouseEvent& event) override;

        void changeListenerCallback(juce::ChangeBroadcaster *source) override;

        static constexpr int textureWidth = 912;
        static constexpr int textureHeight = 313;
    private:
        void uploadTexture();

        std::unique_ptr<juce::VBlankAttachment> vblankAttachment;

        juce::OpenGLContext openGLContext;
        GLuint textureID = 0;
        const uint32_t* textureBuffer = nullptr;
        bool textureNeedsUpdate = false;

        amigaMon::Amiga& amiga;

        std::vector<uint32_t> glBuffer;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    };
} // amigaMon namespace