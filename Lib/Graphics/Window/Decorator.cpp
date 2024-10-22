//
// Created by joao on 10/20/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Decorator.h"
#include "Graphics/OpenGL/Shader.h"
#include "WindowStyles.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/Tools/Resources.h"

namespace Slab::Graphics {
    constexpr int corner_size = 20;

    Decorator::Decorator() : writer(Core::Resources::fontFileName(7), (float)floor(Graphics::title_bar_height*0.6)) {

    }

    void Decorator::decorate(const SlabWindow &slab_window, int x_mouse, int y_mouse) {
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
            auto &bg = Graphics::windowBGColor;

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

            // *** Borders ***
            glBegin(GL_LINE_LOOP);
            {
                auto bc = slab_window.isActive() ? Graphics::windowBorderColor_active
                                                 : Graphics::windowBorderColor_inactive;

                glColor4fv(bc.asFloat4fv());

                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+h);
                glVertex2d(x, y+h);
            }
            glEnd();

            // *** Title bar ***
            glColor4fv(Graphics::titlebar_color.asFloat4fv());
            glBegin(GL_QUADS);
            {
                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+title_bar_height);
                glVertex2d(x, y+title_bar_height);
            }
            glEnd();

            // *** Resize ***
            if(isMouseOverCorner(slab_window, x_mouse, y_mouse)) {
                glBegin(GL_TRIANGLES);
                {
                    glVertex2d(x + w - corner_size, y + h);
                    glVertex2d(x + w, y + h);
                    glVertex2d(x + w, y + h - corner_size);
                }
                glEnd();
            }

            fix h_font = writer.getFontHeightInPixels();
            writer.write(slab_window.getTitle(), {(Real)x+20, syswin_h-(Real)y - .8*h_font}, {1,0,0});
        }
    }

    void Decorator::setSystemWindowShape(int w, int h) {
        syswin_w = w;
        syswin_h = h;

        writer.reshape(w, h);
    }

    bool Decorator::isMouseOverTitlebar(const SlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.getViewport();

        auto x_full = rect.xMin - Graphics::border_size,
             y_full = rect.yMin - Graphics::border_size - Graphics::title_bar_height,
             w_full = rect.width()  + 2*Graphics::border_size;

        return x_mouse>x_full && x_mouse<x_full+w_full && y_mouse>y_full && y_mouse<y_full+Graphics::title_bar_height;    }

    bool Decorator::isMouseOverCorner(const SlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.getViewport();

        auto x_full = rect.xMin - Graphics::border_size,
                y_full = rect.yMin - Graphics::border_size - Graphics::title_bar_height,
                h_full = rect.height() + 2*Graphics::border_size + Graphics::title_bar_height,
                w_full = rect.width()  + 2*Graphics::border_size;

        return x_mouse>x_full+w_full-corner_size
            && x_mouse<x_full+w_full
            && y_mouse<y_full+h_full
            && y_mouse>y_full+h_full-corner_size;
    }

    bool Decorator::isMouseOverGrabRegion(const SlabWindow &window, int x_mouse, int y_mouse) {
        return isMouseOverTitlebar(window, x_mouse, y_mouse) || isMouseOverCorner(window, x_mouse, y_mouse);
    }


} // Slab::Graphics