//
// Created by joao on 3/31/23.
//

#ifndef STUDIOSLAB_OPENGLUTILS_H
#define STUDIOSLAB_OPENGLUTILS_H

#include "../Artists/Rect.h"
#include "Core/Graphics/Window/Window.h"
#include "Core/Graphics/OpenGL/OpenGL.h"

namespace OpenGLUtils {
    /**
     * Checks and logs all stacked OpenGL errors.
     * @param hint Hint to recent operations.
     * @return True if error was found, false otherwise.
     */
    bool checkGLErrors(const Str& hint, bool raiseException=true);

    struct FrameBuffer {
        typedef uint8_t DataType;

        const GLsizei w, h;
        std::vector<DataType> pixels;

        static const GLsizei channels = 4;
        static const GLenum  format   = GL_RGBA,
                type     = GL_UNSIGNED_BYTE;

        int getBitsPerPixel() const {
            assert(FrameBuffer::channels == 4           &&
                   FrameBuffer::format   == GL_RGBA     &&
                   FrameBuffer::type     == GL_UNSIGNED_BYTE );

            return 32;
        }

        const void *getPixelData() const { return &pixels[0]; };
    };

    void drawOrthoNormalized(RectR rect);

    bool outputToPNG(FrameBuffer buffer, std::string fileName);

    bool outputToPNG(Window *window, std::string fileName, int width, int height);

    void piccolos();

    Str GLTypeToGLSLType(GLenum type);
}

#endif //STUDIOSLAB_OPENGLUTILS_H
