//
// Created by joao on 12/3/25.
//

#ifndef STUDIOSLAB_IDRAWBACKEND2D_H
#define STUDIOSLAB_IDRAWBACKEND2D_H
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Plot2D/PlotStyle.h"
#include "Plot2D/Shapes/Shape.h"
#include "Graphics/Types2D.h"
#include "Graphics/Styles/Colors.h"

#include <array>
#include <span>

namespace Slab::Graphics {

namespace OpenGL { class FTexture; }

struct FDrawBackend2DCapabilities {
    bool SupportsTextures = false;
    bool SupportsText = false;
};

struct FColoredVertex2D {
    FPoint2D Position{};
    DevFloat Z = 0.0f;
    FColor Color = Nil;
};

struct FLineSegment2D {
    FPoint2D A{};
    FPoint2D B{};
    DevFloat Z = 0.0f;
};

struct FTexturedVertex2D {
    FPoint2D Position{};
    FPoint2D UV{};
    DevFloat Z = 0.0f;
};

struct IDrawBackend2D {
    virtual ~IDrawBackend2D() = default;

    virtual FDrawBackend2DCapabilities GetCapabilities() const noexcept = 0;

    virtual void BeginFrame(const RectI& viewport, const RectR& view) const noexcept = 0;
    virtual void EndFrame() const noexcept = 0;

    virtual void DrawPointSet(const Math::FPointSet_constptr& pSet, const PlotStyle& style) const noexcept = 0;
    virtual void DrawPointSet(const Math::FPointSet& pSet, const PlotStyle& style) const noexcept = 0;

    virtual void DrawSolidQuad(const FRectangleShape& rect, const FColor& color, DevFloat zIndex) const noexcept = 0;
    virtual void DrawLineSegments(std::span<const FLineSegment2D> segments, const FColor& color, DevFloat width) const noexcept = 0;
    virtual void DrawTriangleStrip(std::span<const FColoredVertex2D> vertices) const noexcept = 0;

    virtual void DrawTexturedQuad(const std::array<FTexturedVertex2D, 4>& vertices,
                                  const FColor& modulation,
                                  const OpenGL::FTexture& texture) const noexcept = 0;

    virtual void DrawVerticalGradientQuad(const FRectangleShape& rect,
                                          const FColor& bottom,
                                          const FColor& top,
                                          DevFloat zIndex) const noexcept = 0;

    virtual void DrawText(const Str& text, const FPoint2D& position, const FColor& color) const noexcept = 0;
};
}

#endif //STUDIOSLAB_IDRAWBACKEND2D_H
