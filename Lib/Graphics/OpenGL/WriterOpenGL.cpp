//
// Created by joao on 10/14/24.
//

#include "WriterOpenGL.h"
#include "Utils.h"
#include "Core/Tools/Resources.h"
#include "Core/Tools/Log.h"

#include <string>
#include <map>
#include <regex>

namespace Slab::Graphics::OpenGL {

    const Str shaderDir = Core::Resources::ShadersFolder + "rougier/";

    typedef struct {
        float x, y, z;    // position
        float s, t;       // texture
        float r, g, b, a; // color
    } vertex_t;

    FWriterOpenGL::FWriterOpenGL(const Str &fontFile, float ptSize, const char *glyphsToPreload)
            : VertexBuffer("vertex:3f,tex_coord:2f,color:4f"),
              Program(shaderDir + "v3f-t2f-c4f.vert", shaderDir + "v3f-t2f-c4f.frag") {

        auto factor = int(ceil(ptSize/30.));

        Atlas = texture_atlas_new(factor*512, factor*512, 1);
        Font = texture_font_new_from_file(Atlas, ptSize, fontFile.c_str());

        if(Font == nullptr) throw Exception(Str("Bad font file: ") + fontFile);

        Core::Log::Critical() << "WriterOpenGL being instantiated. Will start generating atlas now." << Core::Log::Flush;

        FColor white = {1, 1, 1, 1};
        glGenTextures(1, &Atlas->id);

        if(glyphsToPreload != nullptr)
            SetBufferText(glyphsToPreload, {0, 0}, white);

        mat4_set_identity(&m_Projection);
        mat4_set_identity(&m_Model);
        mat4_set_identity(&m_View);

        Core::Log::Success() << "WriterOpenGL with font '" << fontFile << "' with size " << ptSize << "pts instantiated."
                             << Core::Log::Flush;
    }

    FWriterOpenGL::~FWriterOpenGL() {
        glDeleteTextures(1, &Atlas->id);
        Atlas->id = 0;
        texture_atlas_delete(Atlas);

        // "delete vertexBuffer;"

        Str name = Font->filename;
        try {
            Core::Log::Warning() << "TODO: fix problematic font deletion. Font '" << name << "' was not deleted." << Core::Log::Flush;
            //texture_font_delete(font); // TODO: problematik
        } catch(...) {
            Core::Log::Warning() << "Deletion of font '" << name << "' did not go smooth." << Core::Log::Flush;
        }
        Font = nullptr;
    }

    void FWriterOpenGL::UploadAtlas() const
    {
        if(Font==nullptr || !Font->atlas_dirty) return;

        glBindTexture(GL_TEXTURE_2D, Atlas->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei) Atlas->width, (GLsizei) Atlas->height,
                     0, GL_RED, GL_UNSIGNED_BYTE, Atlas->data);

        Font->atlas_dirty = 0;
    }

    void FWriterOpenGL::SetBufferText(const Str &textStr, Point2D pen, FColor Color, bool Rotate90Degrees) {
        VertexBuffer.clear();

        size_t i;
        float r = Color.r, g = Color.g, b = Color.b, a = Color.a;
        auto text = textStr.c_str();
        for (i = 0; i < strlen(text); i += utf8_characterByteSize(text + i)) {
            auto code_point = text + i;

            texture_glyph_t *glyph = texture_font_get_glyph(Font, code_point);

            if(glyph == nullptr){
                Core::Log::Error() << "Graphics::OpenGL::WriterOpenGL could not load glyph '" << code_point << "' "
                                   << "for font '" << Font->filename << "'." << Core::Log::Flush;
            }
            else // if (glyph != nullptr)
            {
                float kerning = 0.0f;
                if (i > 0)
                    kerning = texture_glyph_get_kerning(glyph, text + i - 1);

                if(Rotate90Degrees){
                    pen.y += kerning;

                    const int x0 = (int) (pen.x - glyph->offset_y);
                    const int y0 = (int) (pen.y + glyph->offset_x + glyph->width);
                    const int x1 = (int) (x0 + glyph->height);
                    const int y1 = (int) (y0 - glyph->width);
                    const float s0 = glyph->s0;
                    const float t0 = glyph->t0;
                    const float s1 = glyph->s1;
                    const float t1 = glyph->t1;

                    GLuint indices[6] = {0, 1, 2, 0, 2, 3};
                    vertex_t vertices[4] = {{(float) x0, (float) y0, 0, s1, t0, r, g, b, a},
                                            {(float) x0, (float) y1, 0, s0, t0, r, g, b, a},
                                            {(float) x1, (float) y1, 0, s0, t1, r, g, b, a},
                                            {(float) x1, (float) y0, 0, s1, t1, r, g, b, a}};

                    VertexBuffer.pushBack(vertices, 4, indices, 6);

                    pen.y += glyph->advance_x;
                }
                else {
                    pen.x += kerning;

                    const int x0 = (int) (pen.x + glyph->offset_x);
                    const int y0 = (int) (pen.y + glyph->offset_y);
                    const int x1 = (int) (x0 + glyph->width);
                    const int y1 = (int) (y0 - glyph->height);
                    const float s0 = glyph->s0;
                    const float t0 = glyph->t0;
                    const float s1 = glyph->s1;
                    const float t1 = glyph->t1;

                    GLuint indices[6] = {0, 1, 2, 0, 2, 3};
                    vertex_t vertices[4] = {{(float) x0, (float) y0, 0, s0, t0, r, g, b, a},
                                            {(float) x0, (float) y1, 0, s0, t1, r, g, b, a},
                                            {(float) x1, (float) y1, 0, s1, t1, r, g, b, a},
                                            {(float) x1, (float) y0, 0, s1, t0, r, g, b, a}};

                    VertexBuffer.pushBack(vertices, 4, indices, 6);

                    pen.x += glyph->advance_x;
                }
            }
        }

        if(Font->atlas_dirty) {
            UploadAtlas();
        }
    }

    void FWriterOpenGL::DrawBuffer() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        constexpr auto TextureUnit = GL_TEXTURE0;
        glActiveTexture(TextureUnit);
        glBindTexture(GL_TEXTURE_2D, Atlas->id);

        Program.Use();
        Program.SetUniform("texture", TextureUnit - GL_TEXTURE0);
        Program.SetUniform4x4("model", m_Model.data);
        Program.SetUniform4x4("view", m_View.data);
        Program.SetUniform4x4("projection", m_Projection.data);

        VertexBuffer.Render(GL_TRIANGLES);

    }

    void FWriterOpenGL::Write(const Str &text, Point2D pen, FColor color, bool vertical) {
        if(text.empty()) return;

        SetBufferText(text, pen, color, vertical);
        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " (0)");

        DrawBuffer();
        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " (1)");
    }

    DevFloat FWriterOpenGL::GetFontHeightInPixels() const { return Font->height; }

    void FWriterOpenGL::Reshape(int w, int h) {
        mat4_set_orthographic(&m_Projection, 0, (float) w, 0, (float) h, -1, 1);
    }

    void FWriterOpenGL::Scale(float sx, float sy) {
        mat4_scale(&m_View, sx, sy, 1);
    }

    void FWriterOpenGL::Translate(float dx, float dy) {
        mat4_translate(&m_View, dx, dy, 0);
    }

    void FWriterOpenGL::ResetTransforms() {
        mat4_set_identity(&m_View );
    }
} // Slab::Graphics::OpenGL