#ifndef STUDIOSLAB_PANE_FRAME_UTILS_H
#define STUDIOSLAB_PANE_FRAME_UTILS_H

#include "Graphics/Backend/PlatformWindow.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics::WindowContainer {

    inline auto DrawContentPaneFrame(const FPlatformWindow& PlatformWindow, const FSlabWindow& Window) -> void {
        const auto Rect = Window.GetViewport();
        if (Rect.GetWidth() <= 0 || Rect.GetHeight() <= 0) return;

        OpenGL::Legacy::PushLegacyMode();
        OpenGL::Legacy::PushScene();
        OpenGL::Legacy::ResetModelView();

        const RectI fullRect{0, PlatformWindow.GetWidth(), PlatformWindow.GetHeight(), 0};
        glViewport(0, 0, PlatformWindow.GetWidth(), PlatformWindow.GetHeight());
        OpenGL::Legacy::SetupOrthoI(fullRect);

        const auto BorderColor = WindowStyle::windowBorderColor_inactive;
        glColor4fv(BorderColor.asFloat4fv());
        glLineWidth(1.0f);

        const auto x0 = static_cast<DevFloat>(Rect.xMin) - 0.5;
        const auto y0 = static_cast<DevFloat>(Rect.yMin) - 0.5;
        const auto x1 = static_cast<DevFloat>(Rect.xMax) + 0.5;
        const auto y1 = static_cast<DevFloat>(Rect.yMax) + 0.5;

        glBegin(GL_LINE_LOOP);
        glVertex2d(x0, y0);
        glVertex2d(x1, y0);
        glVertex2d(x1, y1);
        glVertex2d(x0, y1);
        glEnd();

        OpenGL::Legacy::PopScene();
        OpenGL::Legacy::RestoreFromLegacyMode();
    }

} // namespace Slab::Graphics::WindowContainer

#endif // STUDIOSLAB_PANE_FRAME_UTILS_H
