//
// Created by joao on 6/8/24.
//

#include "Shape.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

namespace Slab::Graphics {

FShape::~FShape() {
}

FRectangleShape::FRectangleShape(FPoint2D top_left, FPoint2D bottom_right)
: top_left(top_left), bottom_right(bottom_right) {    }

void FRectangleShape::Draw(const FPlot2DWindow &window) {
    OpenGL::Legacy::FShaderGuard Guard{};

    OpenGL::Legacy::SetupOrtho(window.GetRegion().getRect());
    OpenGL::Legacy::DrawRectangle(*this);
}

FPoint2D FRectangleShape::GetCenter() const {
    const auto xCenter = (top_left.x + bottom_right.x) / 2;
    const auto yCenter = (top_left.y + bottom_right.y) / 2;

    return {xCenter, yCenter};
}

DevFloat FRectangleShape::GetWidth() const {
    return abs(bottom_right.x - top_left.x);
}

DevFloat FRectangleShape::GetHeight() const {
    return abs(bottom_right.y - top_left.y);
}

FRectangleShapeBuilder FRectangleShapeBuilder::WithWidth(const double _w) const {
    return {_w, h, x, y};
}

FRectangleShapeBuilder FRectangleShapeBuilder::WithHeight(const double _h) const {
    return {w, _h, x, y};
}

FRectangleShapeBuilder FRectangleShapeBuilder::At(double _x, double _y) const {
    return {w, h, _x, _y};
}

FRectangleShapeBuilder FRectangleShapeBuilder::At(FPoint2D Loc) const {
    return At(Loc.x, Loc.y);
}

FRectangleShape FRectangleShapeBuilder::Build() const {
    return FRectangleShape({x-w/2, y-h/2}, {x+w/2, y+h/2});
}


} // Slab::Graphics