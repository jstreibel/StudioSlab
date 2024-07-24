//
// Created by joao on 27/08/23.
//

#include "Graphics/OpenGL/OpenGL.h"
#include "GLFreeTypeTests.h"

#include "Core/Tools/Resources.h"

#include "Core/Tools/Log.h"

using namespace Slab;

GLFreeTypeTests::GLFreeTypeTests()
: writer1(Core::Resources::fontFileName(17), 24) // Math symb: 6; 10; 17
, writer2(Core::Resources::fontFileName(7), 22)
{   }

void GLFreeTypeTests::draw() {
    Window::draw();

    glMatrixMode(GL_MODELVIEW);
    fix ratio = Real(getw())/geth();
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

        writer1.write(Str("ℑ(ℱ[ϕ])  ℜ(ℱ[ϕ])  ℐ π² ϵᵢⱼₖ 𝕬𝕭𝕮  𝕬𝕭𝕮"), {40, 120});
        writer1.write(Str("αβγδεζηθικλμνξοπρστυφχψω"), {40, 160});

    }


}

void GLFreeTypeTests::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    auto vp = getViewport();

    writer1.reshape(vp.width(), vp.height());
    writer2.reshape(vp.width(), vp.height());
}
