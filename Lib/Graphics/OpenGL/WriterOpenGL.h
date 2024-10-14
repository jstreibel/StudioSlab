//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_WRITEROPENGL_H
#define STUDIOSLAB_WRITEROPENGL_H

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/Utils/Writer.h"

#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

#include "3rdParty/freetype-gl/demos/mat4.h"
#include "3rdParty/freetype-gl/freetype-gl.h"

namespace Slab::Graphics::OpenGL {

    class WriterOpenGL : public Writer {
        ftgl::texture_font_t *font = nullptr;
        ftgl::texture_atlas_t *atlas = nullptr;

        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::Shader program;

        ftgl::mat4 model{}, view{}, projection{};

        void drawBuffer();
        void setBufferText(const Str &text, Point2D penLocation, Color color=White, bool vertical=false);

        void uploadAtlas();

    public:
        WriterOpenGL() = delete;
        WriterOpenGL(const Str &fontFile, float ptSize, const char *glyphsToPreload=nullptr);
        virtual ~WriterOpenGL();

        void write(const Str &text, Point2D penLocation, Color color=White, bool vertical=false) override;
        Real getFontHeightInPixels() const override;
        void reshape(int w, int h) override;

        void scale(float sx, float sy) override;
        void translate(float dx, float dy) override;
        void resetTransforms() override;
    };

} // Slab::Graphics::OpenGL

#endif //STUDIOSLAB_WRITEROPENGL_H
