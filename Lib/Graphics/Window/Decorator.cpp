//
// Created by joao on 10/20/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/Tools/Resources.h"

namespace Slab::Graphics {
    constexpr int corner_size = 20;


    #define Title_Height (int(WindowStyle::font_size*5./3))

    // Fontes candidatas: 9, 13, 14
    Decorator::Decorator() : writer(Core::Resources::GetIndexedFontFileName(10), (float)WindowStyle::font_size) {

    }

    void Decorator::setup() const {
        // SETUP
        glViewport(0, 0, syswin_w, syswin_h);

        OpenGL::Shader::remove();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glDisable(GL_DEPTH_TEST);
        // glDepthMask(GL_FALSE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        OpenGL::Legacy::SetupOrtho({0, (DevFloat)syswin_w, (DevFloat)syswin_h, 0});
    }

    void Decorator::begin_decoration(const SlabWindow &slab_window, int x_mouse, int y_mouse) {
        auto flags = slab_window.getFlags();
        fix should_clear    = !(flags & SlabWindowDontClear);
        fix should_decorate = !(flags & SlabWindowNoDecoration);

        if(!should_clear) return;

        fix title_height = should_decorate ? Title_Height : 0; // // 34 = 24x => x=34/24
        fix borders_size = should_decorate ? WindowStyle::border_size : 0;

        setup();

        auto rect = slab_window.getViewport();
        auto x = rect.xMin - borders_size,
             y = rect.yMin - borders_size - title_height,
             w = rect.width()  + 2*borders_size,
             h = rect.height() + 2*borders_size + title_height;

        // *** CLEAR ***********************************
        auto &bg = WindowStyle::windowBGColor;

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

    void Decorator::finish_decoration(const SlabWindow &slab_window, int x_mouse, int y_mouse) {
        auto flags = slab_window.getFlags();
        fix should_decorate = !(flags & SlabWindowNoDecoration);

        if(!should_decorate) return;

        fix title_height = Title_Height; // // 34 = 24x => x=34/24
        fix borders_size = WindowStyle::border_size;

        auto rect = slab_window.getViewport();
        auto x = rect.xMin - borders_size,
             y = rect.yMin - borders_size - title_height,
             w = rect.width()  + 2*borders_size,
             h = rect.height() + 2*borders_size + title_height;

        setup();

        // *** DECORATE ********************************

        glLineWidth(2.0f);

        // *** Borders ***
        glBegin(GL_LINE_LOOP);
        {
            auto bc = slab_window.isActive() ? WindowStyle::windowBorderColor_active
                                             : WindowStyle::windowBorderColor_inactive;

            glColor4fv(bc.asFloat4fv());

            glVertex2d(x, y);
            glVertex2d(x+w, y);
            glVertex2d(x+w, y+h);
            glVertex2d(x, y+h);
        }
        glEnd();

        // *** Title bar ***
        glColor4fv(WindowStyle::titlebar_color.asFloat4fv());
        glBegin(GL_QUADS);
        {
            glVertex2d(x, y);
            glVertex2d(x+w, y);
            glVertex2d(x+w, y + Title_Height);
            glVertex2d(x, y + Title_Height);
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
        auto color = Color(32./255,32./255,32./255, 1);
        writer.write(slab_window.getTitle(), {(DevFloat)x+WindowStyle::font_size/2, syswin_h-(DevFloat)y - h_font}, color);
    }

    void Decorator::setSystemWindowShape(int w, int h) {
        syswin_w = w;
        syswin_h = h;

        writer.reshape(w, h);
    }

    bool Decorator::isMouseOverTitlebar(const SlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.getViewport();

        auto x_full = rect.xMin - WindowStyle::border_size,
             y_full = rect.yMin - WindowStyle::border_size - Title_Height,
             w_full = rect.width()  + 2*WindowStyle::border_size;

        return x_mouse>x_full && x_mouse<x_full+w_full && y_mouse>y_full && y_mouse<y_full+Title_Height;    }

    bool Decorator::isMouseOverCorner(const SlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.getViewport();

        auto x_full = rect.xMin - WindowStyle::border_size,
             y_full = rect.yMin - WindowStyle::border_size - Title_Height,
             h_full = rect.height() + 2*WindowStyle::border_size + Title_Height,
             w_full = rect.width()  + 2*WindowStyle::border_size;

        return x_mouse>x_full+w_full-corner_size
            && x_mouse<x_full+w_full
            && y_mouse<y_full+h_full
            && y_mouse>y_full+h_full-corner_size;
    }

    bool Decorator::isMouseOverGrabRegion(const SlabWindow &window, int x_mouse, int y_mouse) {
        return isMouseOverTitlebar(window, x_mouse, y_mouse) || isMouseOverCorner(window, x_mouse, y_mouse);
    }

    int Decorator::titlebar_height() {
        return Title_Height;
    }


} // Slab::Graphics