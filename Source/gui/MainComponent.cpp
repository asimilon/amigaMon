//
// Created by Rich Elmes on 15.11.2024.
//

#include "MainComponent.h"

#include <Emulator.h>

#include <juce_gui_extra/juce_gui_extra.h>

namespace amigaMon {
    //==============================================================================
    MainComponent::MainComponent(amigaMon::Amiga& amigaToUse)
        : amiga(amigaToUse)
    {
        setSize (width * 2, height * 2);

        amiga.start();

        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);

        vblank = std::make_unique<juce::VBlankAttachment>(this, [this]() {
            setTextureData(amiga.getAmiga().emu->getTexture().pixels.ptr);
            repaint();
        });
    }

    MainComponent::~MainComponent()
    {
        openGLContext.detach();
    }

    void MainComponent::paint (juce::Graphics& g)
    {
    }

    void MainComponent::resized()
    {
    }

    void MainComponent::setTextureData(const uint32_t *buffer)
    {
        constexpr int textureWidth = 912;
        constexpr int imageWidth = width * 2;
        constexpr int imageHeight = height * 2;
        constexpr int xOffset = 154 * 2;
        constexpr int yOffset = 36 * 2;

        glBuffer.reserve(imageWidth * imageHeight);

        // Scale and copy ARGB data to the OpenGL-compatible buffer
        for (int y = 0; y < imageHeight; ++y)
        {
            const auto sourceY = (y + yOffset) / 2;
            const auto row = sourceY * textureWidth;

            for (int x = 0; x < imageWidth; ++x)
            {
                const auto sourceX = ((x * 2) + xOffset) / 2;

                uint32_t argb = buffer[row + sourceX];

                // Extract color components (ARGB format)
                uint8_t alpha = (argb >> 24) & 0xFF;
                uint8_t red = (argb >> 16) & 0xFF;
                uint8_t green = (argb >> 8) & 0xFF;
                uint8_t blue = argb & 0xFF;

                uint32_t bgra = (alpha << 24) | (blue << 16) | (green << 8) | red;

                glBuffer[y * imageWidth + x] = bgra;
            }
        }

        this->textureBuffer = glBuffer.data();
        textureNeedsUpdate = true;
    }

    void MainComponent::newOpenGLContextCreated()
    {
        // Generate a texture
        juce::gl::glGenTextures(1, &textureID);
        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, textureID);

        // Set texture parameters
        juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MIN_FILTER, juce::gl::GL_NEAREST);
        juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_MAG_FILTER, juce::gl::GL_NEAREST);
        juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_S, juce::gl::GL_REPEAT);
        juce::gl::glTexParameteri(juce::gl::GL_TEXTURE_2D, juce::gl::GL_TEXTURE_WRAP_T, juce::gl::GL_REPEAT);
    }

    void MainComponent::openGLContextClosing()
    {
        juce::gl::glDeleteTextures(1, &textureID);
    }

    void MainComponent::renderOpenGL()
    {
        if (textureNeedsUpdate && textureBuffer != nullptr)
        {
            uploadTexture();
            textureNeedsUpdate = false;
        }

        // Clear the screen
        juce::gl::glClear(juce::gl::GL_COLOR_BUFFER_BIT | juce::gl::GL_DEPTH_BUFFER_BIT);

        // Enable and bind the texture
        juce::gl::glEnable(juce::gl::GL_TEXTURE_2D);
        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, textureID);

        // Draw a quad with the texture
        juce::gl::glBegin(juce::gl::GL_QUADS);
        juce::gl::glTexCoord2f(0.0f, 1.0f); juce::gl::glVertex2f(-1.0f, -1.0f);
        juce::gl::glTexCoord2f(1.0f, 1.0f); juce::gl::glVertex2f(1.0f, -1.0f);
        juce::gl::glTexCoord2f(1.0f, 0.0f); juce::gl::glVertex2f(1.0f, 1.0f);
        juce::gl::glTexCoord2f(0.0f, 0.0f); juce::gl::glVertex2f(-1.0f, 1.0f);
        juce::gl::glEnd();

        // Disable texture
        juce::gl::glDisable(juce::gl::GL_TEXTURE_2D);
    }

    void MainComponent::uploadTexture()
    {
        // Bind the texture
        juce::gl::glBindTexture(juce::gl::GL_TEXTURE_2D, textureID);

        // Upload the ARGB buffer to the texture
        juce::gl::glTexImage2D(
            juce::gl::GL_TEXTURE_2D,
            0, // Mipmap level
            juce::gl::GL_RGBA, // Internal format
            width * 2,
            height * 2,
            0, // Border
#if JUCE_MAC
            juce::gl::GL_BGRA, // macOS uses BGRA for ARGB buffers
#else
            GL_RGBA, // Other platforms may use RGBA
#endif
            juce::gl::GL_UNSIGNED_BYTE,
            textureBuffer
        );

        amiga.getAmiga().emu->wakeUp();
    }
} // amigaMon namespace