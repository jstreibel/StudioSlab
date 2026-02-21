//
// Created by joao on 27/08/23.
//

#include "Graphics/OpenGL/OpenGL.h"
#include "GLFreeTypeTests.h"

#include "Core/Tools/Resources.h"

#include "Core/Tools/Log.h"

using namespace Slab;

FGLFreeTypeTests::FGLFreeTypeTests()
: Slab::Graphics::FSlabWindow(Graphics::FSlabWindowConfig{"FreeType tests"})
, writer1(Core::Resources::GetIndexedFontFileName(17), 24) // Math symb: 6; 10; 17
, writer2(Core::Resources::GetIndexedFontFileName(7), 22)
{   }

void FGLFreeTypeTests::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
    FSlabWindow::ImmediateDraw(PlatformWindow);

    glMatrixMode(GL_MODELVIEW);
    fix ratio = DevFloat(GetWidth()) / GetHeight();
    glScaled(1/ratio, 1, 1);
    static DevFloat angle = 0.0;
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
        writer1.Write(Str("Angle = ") + ToStr(angle) + Str("deg"), {40, 40});

        writer2.Write(Str("Hell yeah!"), {40, 80});

        writer1.Write(Str("ℑ(ℱ[ϕ])  ℜ(ℱ[ϕ])  ℐ π² ϵᵢⱼₖ 𝕬𝕭𝕮  𝕬𝕭𝕮"), {40, 120});
        writer1.Write(Str("αβγδεζηθικλμνξοπρστυφχψω"), {40, 160});

    }


}

void FGLFreeTypeTests::NotifyReshape(int newWinW, int newWinH) {
    FSlabWindow::NotifyReshape(newWinW, newWinH);

    auto vp = GetViewport();

    writer1.Reshape(vp.GetWidth(), vp.GetHeight());
    writer2.Reshape(vp.GetWidth(), vp.GetHeight());
}
