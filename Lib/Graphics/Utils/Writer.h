//
// Created by joao on 28/08/23.
//

#ifndef STUDIOSLAB_WRITER_H
#define STUDIOSLAB_WRITER_H


#include "3rdParty/freetype-gl/demos/mat4.h"
#include "3rdParty/freetype-gl/freetype-gl.h"

#include "Graphics/Window/Window.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/VertexBuffer.h"

#include "Graphics/Styles/Colors.h"
#include "Graphics/OpenGL/Shader.h"
#include "Utils/EncodingUtils.h"

namespace Slab::Graphics {

    class Writer {
    public:
        Writer() = default;
        virtual ~Writer() = default;

        virtual void write(const Str &text, Point2D penLocation, Color color=White, bool vertical=false) = 0;
        virtual Real getFontHeightInPixels() const = 0;
        virtual void reshape(int w, int h) {};

        virtual void scale(float sx, float sy) {};
        virtual void translate(float dx, float dy) {};
        virtual void resetTransforms() {};
    };

}


#endif //STUDIOSLAB_WRITER_H
