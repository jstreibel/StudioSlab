//
// Created by joao on 6/8/24.
//

#include "ShapeRenderer.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL::Legacy {
    void RenderRectangle(const Slab::Graphics::RectangleShape &rectangleShape) {
        fix tl = rectangleShape.top_left;
        fix br = rectangleShape.bottom_right;

        Shader::remove();

        glBegin(GL_LINE_LOOP);
        {
            glVertex2f(tl.x, tl.y);
            glVertex2f(br.x, tl.y);
            glVertex2f(br.x, br.y);
            glVertex2f(tl.x, br.y);
        }
        glEnd();
    }

}
