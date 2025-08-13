//
// Created by joao on 6/8/24.
//

#include "ShapeRenderer.h"

#include "SceneSetup.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL::Legacy {
    void RenderRectanglee(const RectangleShape &rectangleShape) {
        fix tl = rectangleShape.top_left;
        fix br = rectangleShape.bottom_right;

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
