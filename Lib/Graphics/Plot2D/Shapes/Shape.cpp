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

    RectangleShape::RectangleShape(Point2D top_left, Point2D bottom_right)
    : top_left(top_left), bottom_right(bottom_right) {    }




    void RectangleShape::draw(const FPlot2DWindow &window) {

        OpenGL::Legacy::FShaderGuard Guard{};

        OpenGL::Legacy::SetupOrtho(window.GetRegion().getRect());
        OpenGL::Legacy::DrawRectangle(*this);
    }


} // Slab::Graphics