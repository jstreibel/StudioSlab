//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_GRAPHICS_SHAPE_H
#define STUDIOSLAB_GRAPHICS_SHAPE_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics {

class FPlot2DWindow;

class FShape {
public:
    virtual ~FShape();

private:
    virtual void Draw(const FPlot2DWindow &) = 0;
};

struct FRectangleShape final : FShape {
    FPoint2D top_left, bottom_right;

    FRectangleShape(FPoint2D top_left, FPoint2D bottom_right);

    void Draw(const FPlot2DWindow &window) override;

    FPoint2D GetCenter() const;
    DevFloat GetWidth() const;
    DevFloat GetHeight() const;
};

struct FRectangleShapeBuilder {
    FRectangleShapeBuilder WithWidth(double w) const;
    FRectangleShapeBuilder WithHeight(double h) const;
    FRectangleShapeBuilder At(double x, double y) const;
    FRectangleShapeBuilder At(FPoint2D Loc) const;

    FRectangleShape Build() const;

    DevFloat w, h, x, y;
};

} // Slab::Graphics

#endif // STUDIOSLAB_GRAPHICS_SHAPE_H
