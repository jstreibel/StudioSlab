//
// Created by joao on 9/23/24.
//

#include "LaTeXWriter.h"

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"


namespace Slab::Graphics::LaTeX {

    fix base_w = 512;
    fix base_h = 256;

    typedef struct {
        float x, y, z;    // position
        float s, t;       // texture
        float r, g, b, a; // color
    } vertex_t;

    const Str shaderDir = Core::Resources::ShadersFolder + "rougier/";

    LaTeXWriter::LaTeXWriter()
    : vertexBuffer("vertex:3f,tex_coord:2f,color:4f")
    , program(shaderDir + "v3f-t2f-c4f.vert", shaderDir + "v3f-t2f-c4f.frag") {
        LaTeX::Init();

        cairo_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, base_w, base_h);

        cairo_context = Cairo::Context::create(cairo_surface);

        graphics2D = New<tex::Graphics2D_cairo>(cairo_context);

        texture = New<OpenGL::Texture2D_Color>(base_w, base_h);
        texture->setSWrap(OpenGL::ClampToBorder);
        texture->setTWrap(OpenGL::ClampToBorder);

        const float L = 1;

        const float x0 = -L/2;
        const float y0 = -L/2;
        const float x1 =  L/2;
        const float y1 =  L/2;
        const float s0 = 0.0;
        const float t0 = 0.0;
        const float s1 = 1.0;
        const float t1 = 1.0;
        Color color = {1, 1, 1};
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};
        vertex_t vertices[4] = {{x0, y1, 0, s0, t0, color.r, color.g, color.b, color.a},
                                {x0, y0, 0, s0, t1, color.r, color.g, color.b, color.a},
                                {x1, y0, 0, s1, t1, color.r, color.g, color.b, color.a},
                                {x1, y1, 0, s1, t0, color.r, color.g, color.b, color.a}};

        vertexBuffer.pushBack(vertices, 4, indices, 6);

        ftgl::mat4_set_identity(&projection);
        ftgl::mat4_set_identity(&model);
        ftgl::mat4_set_identity(&view);

        ftgl::mat4_scale(&model, .2, .2, 1.0);
    }

    LaTeXWriter::~LaTeXWriter() {
        LaTeX::Release();
    }

    void LaTeXWriter::renderMath(const WStr& tex_math) {
        auto render = tex::LaTeX::parse(
                tex_math,
                base_w,
                40,
                16,
                0xff000000);

        clearContext();

        render->draw(*graphics2D, 1, 1);

        uploadSurfaceToTexture();
    }

    void LaTeXWriter::uploadSurfaceToTexture() {
        assert(cairo_surface->get_width() == base_w);
        assert(cairo_surface->get_height() == base_h);

        cairo_surface->flush();

        auto data = cairo_surface->get_data();

        constexpr auto element_size = sizeof(int);

        for(int j=0; j<base_h; ++j) {
            for(int i=0; i<base_w; ++i) {
                fix k = (i+j*base_w)*element_size;

                auto color = &data[k];
                auto b = color[0];
                auto g = color[1];
                auto r = color[2];
                auto a = color[3];

                texture->setColor(i, j, Color(a/255.0f, 0, 0, 0));
            }
        }

        texture->upload();
    }

    void LaTeXWriter::draw() {
        //glDisable(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        texture->Bind();

        program.Use();
        program.SetUniform("texture", (GLint) texture->getGLtextureUnit() - GL_TEXTURE0);
        program.SetUniform4x4("model", model.data);
        program.SetUniform4x4("view", view.data);
        program.SetUniform4x4("projection", projection.data);

        vertexBuffer.Render(GL_TRIANGLES);
    }

    void LaTeXWriter::clearContext() {
        cairo_context->set_operator(Cairo::OPERATOR_CLEAR);
        cairo_context->paint();
        cairo_context->set_operator(Cairo::OPERATOR_OVER); // Reset to default operator
    }

} // Slab::Graphics::LaTeX