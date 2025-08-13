//
// Created by joao on 10/20/24.
//

#include "BackgroundArtist.h"

#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Slab::Graphics {
    BackgroundArtist::BackgroundArtist() : FArtist() {
        SetLabel("Background");
    }

    bool BackgroundArtist::Draw(const FPlot2DWindow &window) {

        OpenGL::Legacy::FShaderGuard Guard{};

        OpenGL::Legacy::PushScene();
        OpenGL::Legacy::SetupOrtho({0,1,0,1});

        auto c = PlotThemeManager::GetCurrent()->graphBackground;

        glColor4f(c.r, c.g, c.b, c.a);
        glBegin(GL_QUADS);
        glVertex2f(0,0);
        glVertex2f(1,0);
        glVertex2f(1,1);
        glVertex2f(0,1);
        glEnd();

        OpenGL::Legacy::PopScene();

        return true;
    }

} // Slab::Graphics