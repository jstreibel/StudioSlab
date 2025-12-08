//
// Modern OpenGL backend implementing IDrawBackend2D
//

#ifndef STUDIOSLAB_MODERNDRAWBACKEND2D_H
#define STUDIOSLAB_MODERNDRAWBACKEND2D_H

#include "../../Interfaces/IDrawBackend2D.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/WriterOpenGL.h"

#include <glm/glm.hpp>

namespace Slab::Graphics::OpenGL::Modern {

class FModernDrawBackend2D final : public IDrawBackend2D {
public:
    FModernDrawBackend2D();
    ~FModernDrawBackend2D() override;

    FDrawBackend2DCapabilities GetCapabilities() const noexcept override;

    void BeginFrame(const RectI& viewport, const RectR& view) const noexcept override;
    void EndFrame() const noexcept override;

    void DrawPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) const noexcept override;
    void DrawPointSet(const Math::FPointSet& pSet, const PlotStyle& style) const noexcept override;

    void DrawSolidQuad(const FRectangleShape& rect, const FColor& color, DevFloat zIndex) const noexcept override;
    void DrawLineSegments(std::span<const FLineSegment2D> segments, const FColor& color, DevFloat width) const noexcept override;
    void DrawTriangleStrip(std::span<const FColoredVertex2D> vertices) const noexcept override;

    void DrawTexturedQuad(const std::array<FTexturedVertex2D, 4>& vertices,
                          const FColor& modulation,
                          const OpenGL::FTexture& texture) const noexcept override;

    void DrawVerticalGradientQuad(const FRectangleShape& rect,
                                  const FColor& bottom,
                                  const FColor& top,
                                  DevFloat zIndex) const noexcept override;

    void DrawText(const Str& text, const FPoint2D& position, const FColor& color) const noexcept override;

private:
    struct FColorVertex {
        glm::vec2 Pos{};
        float Z = 0.0f;
        glm::vec4 Color{1.0f};
    };

    struct FTexVertex {
        glm::vec2 Pos{};
        float Z = 0.0f;
        glm::vec2 UV{};
    };

    GLuint ColorProgram = 0;
    GLuint TextureProgram = 0;
    GLuint ColorVAO = 0;
    GLuint ColorVBO = 0;
    GLuint TexVAO = 0;
    GLuint TexVBO = 0;

    mutable glm::mat4 Projection{};
    mutable RectR View{};
    mutable RectI Viewport{};

    mutable TPointer<FWriterOpenGL> Writer = nullptr;

    void InitColorPipeline();
    void InitTexturePipeline();

    void DrawColor(GLenum mode, const std::vector<FColorVertex>& vertices, float lineWidth = 1.0f) const;
    void DrawTexture(GLenum mode, const std::vector<FTexVertex>& vertices,
                     const FColor& modulation,
                     const OpenGL::FTexture& texture) const;

    void EnsureWriter() const;
};

} // Slab::Graphics::OpenGL::Modern

#endif // STUDIOSLAB_MODERNDRAWBACKEND2D_H
