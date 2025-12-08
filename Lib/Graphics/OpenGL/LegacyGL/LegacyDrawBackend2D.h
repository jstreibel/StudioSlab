//
// Legacy OpenGL backend implementing IDrawBackend2D
//

#ifndef STUDIOSLAB_LEGACYDRAWBACKEND2D_H
#define STUDIOSLAB_LEGACYDRAWBACKEND2D_H

#include "Graphics/IDrawBackend2D.h"
#include "Graphics/OpenGL/Texture.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

namespace Slab::Graphics::OpenGL::Legacy {

class FLegacyDrawBackend2D final : public IDrawBackend2D {
public:
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
                          const FTexture& texture) const noexcept override;
    void DrawVerticalGradientQuad(const FRectangleShape& rect,
                                  const FColor& bottom,
                                  const FColor& top,
                                  DevFloat zIndex) const noexcept override;

    void DrawText(const Str& text, const FPoint2D& position, const FColor& color) const noexcept override;

private:
    static void ApplyColor(const FColor& color);
};

} // Slab::Graphics::OpenGL::Legacy

#endif // STUDIOSLAB_LEGACYDRAWBACKEND2D_H
