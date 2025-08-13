//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_WRITER_OPENGL_H
#define STUDIOSLAB_WRITER_OPENGL_H

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/Utils/Writer.h"

#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"

#include "3rdParty/freetype-gl/demos/mat4.h"
#include "3rdParty/freetype-gl/freetype-gl.h"

namespace Slab::Graphics::OpenGL {

    class FWriterOpenGL final : public FWriter {
        ftgl::texture_font_t *Font = nullptr;
        ftgl::texture_atlas_t *Atlas = nullptr;

        OpenGL::FVertexBuffer VertexBuffer;
        OpenGL::FShader Program;

        ftgl::mat4 m_Model{}, m_View{}, m_Projection{};

        void DrawBuffer();
        void SetBufferText(const Str &Text, Point2D PenLocation, FColor Color=White, bool Rotate90Degrees=false);

        void UploadAtlas() const;

    public:
        FWriterOpenGL() = delete;
        FWriterOpenGL(const Str &fontFile, float ptSize, const char *glyphsToPreload=nullptr);
        ~FWriterOpenGL() override;

        void Write(const Str& text, Point2D penLocation, FColor color = White,
                   bool vertical = false) override;
        [[nodiscard]] DevFloat GetFontHeightInPixels() const override;
        void Reshape(int w, int h) override;

        void Scale(float sx, float sy) override;
        void Translate(float dx, float dy) override;
        void ResetTransforms() override;
    };

} // Slab::Graphics::OpenGL

#endif //STUDIOSLAB_WRITER_OPENGL_H
