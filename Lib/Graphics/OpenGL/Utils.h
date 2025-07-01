//
// Created by joao on 3/31/23.
//

#ifndef STUDIOSLAB_OPENGLUTILS_H
#define STUDIOSLAB_OPENGLUTILS_H

#include "Graphics/Types2D.h"
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/OpenGL/OpenGL.h"


namespace Slab::Graphics::OpenGL {

        /**
     * Checks and logs all stacked OpenGL errors.
     * @param hint Hint to recent operations.
     * @return True if error was found, false otherwise.
     */
    bool CheckGLErrors(const Str& hint, bool raiseException=false);

    struct FrameBuffer {
        typedef uint8_t DataType;

        const GLsizei w, h;
        Vector<DataType> pixels;

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



    void DrawOrthoNormalized(RectR rect);

    bool OutputToPNG(FrameBuffer buffer, std::string fileName);

    bool OutputToPNG(FSlabWindow *window, std::string fileName, int width, int height);

    void piccolos();

    Str GLTypeToGLSLType(GLenum type);
}

#endif //STUDIOSLAB_OPENGLUTILS_H
