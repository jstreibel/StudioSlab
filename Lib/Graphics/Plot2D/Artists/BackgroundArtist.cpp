//
// Created by joao on 10/20/24.
//

#include "BackgroundArtist.h"

#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Plot2D/PlotThemeManager.h"

namespace Slab::Graphics {
    BackgroundArtist::BackgroundArtist() {
        setLabel("Background");
    }

    bool BackgroundArtist::draw(const Plot2DWindow &window) {

        OpenGL::Legacy::SetupOrtho({0,1,0,1});

        auto c = PlotThemeManager::GetCurrent()->graphBackground;

        glColor4f(c.r, c.g, c.b, c.a);
        glBegin(GL_QUADS);
        glVertex2f(0,0);
        glVertex2f(1,0);
        glVertex2f(1,1);
        glVertex2f(0,1);
        glEnd();

        return true;
    }

} // Slab::Graphics