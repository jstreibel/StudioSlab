//
// Created by joao on 9/23/24.
//

#ifndef STUDIOSLAB_LATEXWRITER_H
#define STUDIOSLAB_LATEXWRITER_H

#include "3rdParty/NanoTeX/src/latex.h"
#include "3rdParty/freetype-gl/demos/mat4.h"

#include "LaTeX.h"
#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

#include "Utils/String.h"
#include "Utils/Pointer.h"


namespace Slab::Graphics::LaTeX {

    class LaTeXWriter {
        Pointer<tex::Graphics2D> graphics2D;

        OpenGL::VertexBuffer vertexBuffer;
        OpenGL::FShader program;
        Pointer<OpenGL::Texture2D_Color> texture;

        Cairo::RefPtr<Cairo::ImageSurface> cairo_surface;
        Cairo::RefPtr<Cairo::Context> cairo_context;

        ftgl::mat4 model{}, view{}, projection{};

        void uploadSurfaceToTexture();
        void clearContext();
    public:
        explicit LaTeXWriter();

        ~LaTeXWriter();

        void renderMath(const WStr& tex_math);

        void draw();
    };

} // Slab::Graphics::LaTeX

#endif //STUDIOSLAB_LATEXWRITER_H
