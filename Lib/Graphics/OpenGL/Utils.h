//
// Created by joao on 3/31/23.
//

#ifndef STUDIOSLAB_OPENGLUTILS_H
#define STUDIOSLAB_OPENGLUTILS_H

#include "Graphics/Types.h"
#include "Graphics/Window/Window.h"
#include "Graphics/OpenGL/OpenGL.h"


namespace Slab::Graphics::OpenGL {
    /**
     * Checks and logs all stacked OpenGL errors.
     * @param hint Hint to recent operations.
     * @return True if error was found, false otherwise.
     */
    bool checkGLErrors(const Str& hint, bool raiseException=false);

    struct FrameBuffer {
        typedef uint8_t DataType;

        const GLsizei w, h;
        std::vector<DataType> pixels;

        static const GLsizei channels = 4;
        static const GLenum  format   = GL_RGBA,
                type     = GL_UNSIGNED_BYTE;

        static int GetBitsPerPixel() {
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
