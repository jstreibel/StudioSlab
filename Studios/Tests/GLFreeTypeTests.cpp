//
// Created by joao on 27/08/23.
//

#include "Core/Graphics/OpenGL/OpenGL.h"
#include "GLFreeTypeTests.h"

#include "Utils/Resources.h"

#include "Core/Tools/Log.h"

GLFreeTypeTests::GLFreeTypeTests()
: writer1(Resources::fontFileName(9), 24)
, writer2(Resources::fontFileName(7), 22)
{   }

void GLFreeTypeTests::draw() {
    Window::draw();

    glMatrixMode(GL_MODELVIEW);
    fix ratio = Real(w)/h;
    glScaled(1/ratio, 1, 1);
    static Real angle = 0.0;
    glRotated(angle, 0, 0, 1);
    angle+=0.1;


    {
        // glColor3d(.6, .6, .7);
        glBegin(GL_LINES);
        glVertex2d(-0.25, .0);
        glVertex2d(0.25, .0);
        glVertex2d(.0, -0.25);
        glVertex2d(.0, 0.25);
        glEnd();
    }

    {
        writer1.write(Str("Angle = ") + ToStr(angle) + Str("deg"), {40, 40});

        writer2.write(Str("Hell yeah!"), {40, 80});
    }


}

void GLFreeTypeTests::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    auto vp = getViewport();

    writer1.reshape(vp.w(), vp.h());
    writer2.reshape(vp.w(), vp.h());
}
