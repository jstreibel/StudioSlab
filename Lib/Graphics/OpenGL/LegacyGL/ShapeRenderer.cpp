//
// Created by joao on 6/8/24.
//

#include "ShapeRenderer.h"

#include "SceneSetup.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL::Legacy {
    void SetColor(const FColor& Color) {
        glColor4f(Color.r, Color.g, Color.b, Color.a);
    }

    void DrawRectangle(const RectangleShape &rectangleShape) {
        fix tl = rectangleShape.top_left;
        fix br = rectangleShape.bottom_right;

        glBegin(GL_QUADS);
        {
            glVertex2f(tl.x, tl.y);
            glVertex2f(br.x, tl.y);
            glVertex2f(br.x, br.y);
            glVertex2f(tl.x, br.y);
        }
        glEnd();
    }

    void DrawLine(const Real2D& start, const Real2D& end, const FColor& color, const DevFloat &thickness) {
        SetColor(color);

        glLineWidth(thickness);

        glBegin(GL_LINES);
        glVertex2d(start.x, start.y);
        glVertex2d(end.x, end.y);
        glEnd();
    }

}
