//
// Created by joao on 28/08/23.
//

#ifndef STUDIOSLAB_WRITER_H
#define STUDIOSLAB_WRITER_H


#include "3rdParty/freetype-gl/vertex-buffer.h"
#include "3rdParty/freetype-gl/demos/mat4.h"
#include "3rdParty/freetype-gl/freetype-gl.h"

#include "Core/Graphics/Window/Window.h"

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "Core/Graphics/OpenGL/VertexBuffer.h"

#include "Core/Graphics/Styles/Colors.h"
#include "Shader.h"

namespace Core::Graphics {

    class Writer {
        ftgl::texture_font_t *font = nullptr;
        ftgl::texture_atlas_t *atlas = nullptr;

        ftgl::vertex_buffer_t *buffer;
        GLuint shader;
        // OpenGL::VertexBuffer vertexBuffer;
        // OpenGL::Shader program;

        ftgl::mat4 model{}, view{}, projection{};

        void drawBuffer();
        void setBufferText(const Str &text, Point2D penLocation, Styles::Color color=Styles::White);

    public:
        Writer(const Str &fontFile, float ptSize);
        virtual ~Writer();

        void write(const Str &text, Point2D penLocation, Styles::Color color=Styles::White);
        Real getFontHeightInPixels() const;
        void reshape(int w, int h);



    };

}


#endif //STUDIOSLAB_WRITER_H
