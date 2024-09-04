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
#include "Shader.h"

namespace Slab::Graphics {

    class Writer {
        ftgl::texture_font_t *font = nullptr;
        ftgl::texture_atlas_t *atlas = nullptr;

        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::Shader program;

        ftgl::mat4 model{}, view{}, projection{};

        void drawBuffer();
        void setBufferText(const Str &text, Point2D penLocation, Color color=White, bool vertical=false);

    public:
        Writer() = delete;
        Writer(const Str &fontFile, float ptSize);
        virtual ~Writer();

        void write(const Str &text, Point2D penLocation, Color color=White, bool vertical=false);
        Real getFontHeightInPixels() const;
        void reshape(int w, int h);



    };

}


#endif //STUDIOSLAB_WRITER_H
