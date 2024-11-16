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
        amiga.start();

        openGLContext.setRenderer(this);
        openGLContext.attachTo(*this);

        vblankAttachment = std::make_unique<juce::VBlankAttachment>(this, [this]() {
            if(amiga.shouldAdvanceFrame.exchange(false))
            {
                amiga.getAmiga().resume();
                amiga.getAmiga().wakeUp();
                amiga.shouldPause.store(true);
            }
            else if(amiga.shouldPause.exchange(false))
            {
                amiga.getAmiga().suspend();
            }
            if(!amiga.getAmiga().isPaused())
            {
                setTextureData(amiga.getAmiga().emu->getTexture().pixels.ptr);
                repaint();
            }
        });

        setSize (width * 2, height * 2);
    }

    MainComponent::~MainComponent()
    {
        vblankAttachment.reset();
        openGLContext.detach();
    }

    void MainComponent::setTextureData(const uint32_t *buffer)
    {
        constexpr int textureWidth = 912;
        constexpr int imageWidth = width * 2;
        constexpr int imageHeight = height * 2;
        constexpr int xOffset = 156 * 2;
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

#if JUCE_MAC
                // Extract color components (ARGB format)
                uint8_t alpha = (argb >> 24) & 0xFF;
                uint8_t red = (argb >> 16) & 0xFF;
                uint8_t green = (argb >> 8) & 0xFF;
                uint8_t blue = argb & 0xFF;
                uint32_t bgra = (alpha << 24) | (blue << 16) | (green << 8) | red;
                glBuffer[y * imageWidth + x] = bgra;
#else
                uint8_t alpha = (argb >> 24) & 0xFF;
                uint32_t rgb = argb & 0x00FFFFFF << 8;
                uint32_t rgba = rgb | alpha;
                glBuffer[y * imageWidth + x] = rgba;
#endif
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

    void MainComponent::mouseUp(const juce::MouseEvent &event)
    {
        if(event.mods.isLeftButtonDown())
        {
            amiga.getAmiga().controlPort1.mouse.trigger(RELEASE_LEFT);
        }
        else if(event.mods.isRightButtonDown())
        {
            amiga.getAmiga().controlPort1.mouse.trigger(RELEASE_RIGHT);
        }
    }

    void MainComponent::mouseDown(const juce::MouseEvent &event)
    {
        if(event.mods.isLeftButtonDown())
        {
            amiga.getAmiga().controlPort1.mouse.trigger(PRESS_LEFT);
        }
        else if(event.mods.isRightButtonDown())
        {
            amiga.getAmiga().controlPort1.mouse.trigger(PRESS_RIGHT);
        }
    }

    void MainComponent::mouseMove(const juce::MouseEvent &event)
    {
        amiga.getAmiga().controlPort1.mouse.setXY(event.position.x, event.position.y);
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

        if(!amiga.getAmiga().isPaused())
            amiga.getAmiga().emu->wakeUp();
    }
} // amigaMon namespace