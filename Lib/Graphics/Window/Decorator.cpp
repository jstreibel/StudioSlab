//
// Created by joao on 10/20/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Decorator.h"
#include "Graphics/OpenGL/Shader.h"
#include "WindowStyles.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"


namespace Slab::Graphics {
    void Decorator::operator()(const SlabWindow &slab_window) {
        auto rect = slab_window.getViewport();
        auto flags = slab_window.getFlags();

        fix should_clear    = !(flags & SlabWindow::DontClear);
        fix should_decorate = !(flags & SlabWindow::NoDecoration);

        int xtra_padding = slab_window.hasMainMenu() ? Graphics::menuHeight : 0;
        fix title_height = should_decorate ? Graphics::title_bar_height : 0;


        auto x = rect.xMin - Graphics::border_size,
             y = rect.yMin - Graphics::border_size - Graphics::title_bar_height,
             w = rect.width()  + 2*Graphics::border_size,
             h = rect.height() + 2*Graphics::border_size + xtra_padding + title_height;

        // SETUP
        glViewport(0, 0, syswin_w, syswin_h);

        OpenGL::Shader::remove();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        OpenGL::Legacy::SetupOrtho({0, (Real)syswin_w, (Real)syswin_h, 0});

        // *** CLEAR ***********************************
        if(should_clear) {
            auto &bg = Graphics::clearColor;

            glBegin(GL_QUADS);
            {
                glColor4d(bg.r, bg.g, bg.b, bg.a);
                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+h);
                glVertex2d(x, y+h);
            }
            glEnd();
        }

        // *** DECORATE ********************************
        if(should_decorate) {
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            {
                auto bc = slab_window.isActive() ? Graphics::windowBorderColor_active
                                                 : Graphics::windowBorderColor_inactive;

                glColor4d(bc.r, bc.g, bc.b, bc.a);

                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+h);
                glVertex2d(x, y+h);
            }
            glEnd();

            glColor3f(.5, .5, 1);
            glBegin(GL_QUADS);
            {
                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+title_bar_height);
                glVertex2d(x, y+title_bar_height);
            }
            glEnd();
        }
    }

    void Decorator::setSystemWindowShape(int w, int h) {
        syswin_w = w;
        syswin_h = h;
    }
} // Slab::Graphics