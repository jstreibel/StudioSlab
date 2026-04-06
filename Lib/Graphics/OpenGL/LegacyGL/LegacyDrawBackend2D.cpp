//
// Legacy OpenGL backend implementing IDrawBackend2D
//

#include "LegacyDrawBackend2D.h"

#include "LegacyMode.h"
#include "SceneSetup.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/WriterOpenGL.h"
#include "Core/Tools/Resources.h"
#include "Utils/Pointer.h"

namespace Slab::Graphics::OpenGL::Legacy {

namespace {
    constexpr auto k_DefaultLineWidth = 1.0f;
}

FDrawBackend2DCapabilities FLegacyDrawBackend2D::GetCapabilities() const noexcept {
    return {
        .SupportsTextures = true,
        .SupportsText = true,
    };
}

void FLegacyDrawBackend2D::BeginFrame(const RectI& viewport, const RectR& view) const noexcept {
    SetViewport(viewport);
    SetupLegacyGL();
    ResetModelView();
    SetupOrtho(view);
}

void FLegacyDrawBackend2D::EndFrame() const noexcept { }

void FLegacyDrawBackend2D::DrawPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) const noexcept {
    RenderPointSet(pSet, style);
}

void FLegacyDrawBackend2D::DrawPointSet(const Math::FPointSet& pSet, const PlotStyle& style) const noexcept {
    DrawPointSet(Dummy(pSet), style);
}

void FLegacyDrawBackend2D::DrawSolidQuad(const FRectangleShape& rect, const FColor& color, DevFloat zIndex) const noexcept {
    SetupLegacyGL();
    ::glDisable(GL_TEXTURE_2D);

    ApplyColor(color);

    ::glBegin(GL_QUADS);
    ::glVertex3f(rect.top_left.x, rect.top_left.y, zIndex);
    ::glVertex3f(rect.bottom_right.x, rect.top_left.y, zIndex);
    ::glVertex3f(rect.bottom_right.x, rect.bottom_right.y, zIndex);
    ::glVertex3f(rect.top_left.x, rect.bottom_right.y, zIndex);
    ::glEnd();

}

void FLegacyDrawBackend2D::DrawLineSegments(std::span<const FLineSegment2D> segments,
                                            const FColor& color,
                                            DevFloat width) const noexcept {
    if (segments.empty()) return;

    SetupLegacyGL();
    ::glDisable(GL_TEXTURE_2D);
    ::glEnable(GL_BLEND);
    ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ::glLineWidth(width > 0.0f ? width : k_DefaultLineWidth);

    ApplyColor(color);
    ::glBegin(GL_LINES);
    for (const auto& segment : segments) {
        ::glVertex3f(segment.A.x, segment.A.y, segment.Z);
        ::glVertex3f(segment.B.x, segment.B.y, segment.Z);
    }
    ::glEnd();

}

void FLegacyDrawBackend2D::DrawTriangleStrip(std::span<const FColoredVertex2D> vertices) const noexcept {
    if (vertices.size() < 3) return;

    SetupLegacyGL();
    ::glDisable(GL_TEXTURE_2D);
    ::glEnable(GL_BLEND);
    ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ::glBegin(GL_TRIANGLE_STRIP);
    for (const auto& v : vertices) {
        ApplyColor(v.Color);
        ::glVertex3f(v.Position.x, v.Position.y, v.Z);
    }
    ::glEnd();

}

void FLegacyDrawBackend2D::DrawTexturedQuad(const std::array<FTexturedVertex2D, 4>& vertices,
                                            const FColor& modulation,
                                            const FTexture& texture) const noexcept {
    SetupLegacyGL();
    FTexture::EnableTextures();
    texture.Activate();
    texture.Bind();

    ApplyColor(modulation);

    ::glBegin(GL_QUADS);
    for (const auto& v : vertices) {
        ::glTexCoord2f(v.UV.x, v.UV.y);
        ::glVertex3f(v.Position.x, v.Position.y, v.Z);
    }
    ::glEnd();

    FTexture::Deactivate();
    FTexture::DisableTextures();

}

void FLegacyDrawBackend2D::DrawVerticalGradientQuad(const FRectangleShape& rect,
                                                    const FColor& bottom,
                                                    const FColor& top,
                                                    DevFloat zIndex) const noexcept {
    SetupLegacyGL();
    ::glDisable(GL_TEXTURE_2D);
    ::glEnable(GL_BLEND);
    ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ::glBegin(GL_TRIANGLE_STRIP);
    ApplyColor(bottom);
    ::glVertex3f(rect.top_left.x, rect.bottom_right.y, zIndex);
    ::glVertex3f(rect.bottom_right.x, rect.bottom_right.y, zIndex);
    ApplyColor(top);
    ::glVertex3f(rect.top_left.x, rect.top_left.y, zIndex);
    ::glVertex3f(rect.bottom_right.x, rect.top_left.y, zIndex);
    ::glEnd();

}

void FLegacyDrawBackend2D::DrawText(const Str& text, const FPoint2D& position, const FColor& color) const noexcept {
    static auto Writer = Slab::New<FWriterOpenGL>(Slab::Core::Resources::BuiltinFonts::JuliaMono_Regular(), 28);
    ApplyColor(color);
    Writer->Write(text, position, color);
    FTexture::Deactivate();
    FShader::LegacyGL();
}

void FLegacyDrawBackend2D::ApplyColor(const FColor& color) {
    ::glColor4f(color.r, color.g, color.b, color.a);
}

} // Slab::Graphics::OpenGL::Legacy
