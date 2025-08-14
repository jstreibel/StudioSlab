//
// Created by joao on 10/20/24.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"

namespace Slab::Graphics {
    constexpr int corner_size = 20;


    #define Title_Height (int(WindowStyle::font_size*5./3))

    // Fontes candidatas: 9, 13, 14
    FDecorator::FDecorator() : Writer(Core::Resources::GetIndexedFontFileName(10), (float)WindowStyle::font_size) {

    }

    void FDecorator::Setup() const {
        // SETUP
        glViewport(0, 0, SysWin_Width, SysWin_Height);

        glDisable(GL_TEXTURE_2D);

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glDisable(GL_DEPTH_TEST);
        // glDepthMask(GL_FALSE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        OpenGL::Legacy::SetupOrtho({0, static_cast<DevFloat>(SysWin_Width), static_cast<DevFloat>(SysWin_Height), 0});

        OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
    }

    void FDecorator::BeginDecoration(const FSlabWindow &SlabWindow, int x_mouse, int y_mouse) {
        fix Flags = SlabWindow.GetFlags();
        fix ShouldClear    = !(Flags & SlabWindowDontClear);
        fix ShouldDecorate = !(Flags & SlabWindowNoDecoration);

        if(!ShouldClear) return;

        {
            OpenGL::Legacy::FShaderGuard Guard{};

            fix TitleHeight = ShouldDecorate ? Title_Height : 0;
            fix BordersSize = ShouldDecorate ? WindowStyle::BorderSize : 0;

            Setup();

            auto rect = SlabWindow.GetViewport();
            auto x = rect.xMin - BordersSize,
                 y = rect.yMin - BordersSize - TitleHeight,
                 w = rect.GetWidth()  + 2*BordersSize,
                 h = rect.GetHeight() + 2*BordersSize + TitleHeight;

            // *** CLEAR ***********************************
            auto &BackgroundColor = WindowStyle::WindowBGColor;

            glBegin(GL_QUADS);
            {
                glColor4d(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, BackgroundColor.a);
                glVertex2d(x, y);
                glVertex2d(x+w, y);
                glVertex2d(x+w, y+h);
                glVertex2d(x, y+h);
            }
            glEnd();
        }
    }

    void FDecorator::FinishDecoration(const FSlabWindow &SlabWindow, int x_mouse, int y_mouse) {
        fix Flags = SlabWindow.GetFlags();

        if(fix ShouldDecorate = !(Flags & SlabWindowNoDecoration); !ShouldDecorate) return;

        fix TitleHeight = Title_Height; // // 34 = 24x => x=34/24
        fix BordersSize = WindowStyle::BorderSize;

        fix Viewport = SlabWindow.GetViewport();
        fix x = Viewport.xMin - BordersSize,
            y = Viewport.yMin - BordersSize - TitleHeight,
            w = Viewport.GetWidth()  + 2*BordersSize,
            h = Viewport.GetHeight() + 2*BordersSize + TitleHeight;

        OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
        if constexpr (true)
        {
            OpenGL::Legacy::FShaderGuard Guard{};

            Setup();

            // *** DECORATE ********************************

            glLineWidth(2.0f);

            // *** Borders ***
            glBegin(GL_LINE_LOOP);
            {
                auto bc = SlabWindow.IsActive() ? WindowStyle::windowBorderColor_active
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
            if(IsMouseOverCorner(SlabWindow, x_mouse, y_mouse)) {
                glBegin(GL_TRIANGLES);
                {
                    glVertex2d(x + w - corner_size, y + h);
                    glVertex2d(x + w, y + h);
                    glVertex2d(x + w, y + h - corner_size);
                }
                glEnd();
            }
            OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
        }
        OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));

        if constexpr (true)
        {
            fix FontHeightInPixels = Writer.GetFontHeightInPixels();
            fix FontColor = FColor(32./255,32./255,32./255, 1);

            Writer.Write(SlabWindow.GetTitle(),
                {
                    static_cast<DevFloat>(x)+WindowStyle::font_size/2,
                    SysWin_Height-static_cast<DevFloat>(y) - FontHeightInPixels
                },
                FontColor);
        }
    }

    void FDecorator::SetSystemWindowShape(const int w, const int h) {
        SysWin_Width = w;
        SysWin_Height = h;

        Writer.Reshape(w, h);
    }

    bool FDecorator::IsMouseOverTitlebar(const FSlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.GetViewport();

        auto x_full = rect.xMin - WindowStyle::BorderSize,
             y_full = rect.yMin - WindowStyle::BorderSize - Title_Height,
             w_full = rect.GetWidth()  + 2*WindowStyle::BorderSize;

        return x_mouse>x_full && x_mouse<x_full+w_full && y_mouse>y_full && y_mouse<y_full+Title_Height;    }

    bool FDecorator::IsMouseOverCorner(const FSlabWindow &window, int x_mouse, int y_mouse) {
        fix rect = window.GetViewport();

        auto x_full = rect.xMin - WindowStyle::BorderSize,
             y_full = rect.yMin - WindowStyle::BorderSize - Title_Height,
             h_full = rect.GetHeight() + 2*WindowStyle::BorderSize + Title_Height,
             w_full = rect.GetWidth()  + 2*WindowStyle::BorderSize;

        return x_mouse>x_full+w_full-corner_size
            && x_mouse<x_full+w_full
            && y_mouse<y_full+h_full
            && y_mouse>y_full+h_full-corner_size;
    }

    bool FDecorator::IsMouseOverGrabRegion(const FSlabWindow &window, int x_mouse, int y_mouse) {
        return IsMouseOverTitlebar(window, x_mouse, y_mouse) || IsMouseOverCorner(window, x_mouse, y_mouse);
    }

    int FDecorator::TitlebarHeight() {
        return Title_Height;
    }


} // Slab::Graphics