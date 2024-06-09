//
// Created by joao on 6/8/24.
//

#include "Shape.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"

namespace Slab::Graphics {

    RectangleShape::RectangleShape(Point2D top_left, Point2D bottom_right)
    : top_left(top_left), bottom_right(bottom_right) {    }




    void RectangleShape::draw(const PlottingWindow &window) {
        ::Slab::Graphics::OpenGL::Legacy::RenderRectangle(*this);
    }


} // Slab::Graphics