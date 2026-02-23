//
// Created by joao on 20/08/23.
//

#include "WindowColumn.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Backend/PlatformWindow.h"
#include "Core/Backend/BackendManager.h"

#include <algorithm>
#include <numeric>

#define CountLessThanZero(vec) std::count_if(vec.begin(), vec.end(), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(vec.begin(), vec.end(), 0.0f, [](float acc, float x) { return x > 0 ? acc + x : acc; });


// Don't touch
fix AlwaysPropagate = true;
fix PropagateOnlyIfMouseIsIn = false;

#define PropagateEvent(EVENT, PROPAGATE_ALWAYS){                         \
    auto responded = false;                                              \
                                                                         \
    responded = FSlabWindow::EVENT;                                       \
                                                                         \
    for(auto &win : Windows)                                             \
        if(win->IsMouseInside() || PROPAGATE_ALWAYS) responded = win->EVENT; \
                                                                         \
    return responded;                                                    \
}


namespace Slab::Graphics {

    using namespace Core;

    namespace {
        auto DrawContentPaneFrame(const FPlatformWindow& PlatformWindow, const FSlabWindow& Window) -> void {
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
    } // namespace

    void FWindowColumn::addWindow(const TPointer<FSlabWindow>& window, float windowHeight) {
        Windows.emplace_back(window);
        heights.emplace_back(windowHeight);
    }

    bool FWindowColumn::removeWindow(const TPointer<FSlabWindow>& window) {

        auto it = Windows.begin();
        for(auto i=0; i<Windows.size(); ++i){
            if(*it == window) {
                Windows.remove(*it);
                heights.erase(heights.begin()+i);

                return true;
            }

            it++;
        }

        return false;
    }

    void FWindowColumn::arrangeWindows() {
        if (!assertConsistency()) throw Exception("WindowRow inconsistency");

        auto m = Windows.size();

        if (m == 0) return;

        const auto gap = WindowStyle::TilingGapSize;
        const auto innerWidth = std::max(0, GetWidth() - 2 * gap);
        const auto innerHeight = std::max(0, GetHeight() - static_cast<int>((m + 1) * gap));

        Vector<int> computedHeights(m, m > 0 ? (int) (innerHeight / m) : 0);    // "if(freeHeights==m)"

        auto freeHeights = CountLessThanZero(heights)
        if (freeHeights == 0) {
            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = innerHeight * relHeight;
                computedHeights[i] = (int) height;
            }
        } else if (freeHeights != m) {
            auto reservedHeight = SumLargerThanZero(heights)
            auto hFree = (float) innerHeight * (1 - reservedHeight) / (float) freeHeights;

            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = relHeight > 0 ? innerHeight * relHeight : hFree;
                computedHeights[i] = (int) height;
            }
        }

        Vector<int> computed_yPositions(m);
        auto y = this->Get_y() + gap;
        for (int i = 0; i < m; ++i) {
            computed_yPositions[i] = y;
            y += computedHeights[i] + gap;
        }

        auto i = 0;
        for (auto &win: Windows) {
            win->Set_x(Get_x() + gap);
            win->Set_y(computed_yPositions[i]);

            win->NotifyReshape(innerWidth, std::max(0, computedHeights[i]));

            i++;
        }
    }

    bool FWindowColumn::assertConsistency() const {
        auto reservedHeight = SumLargerThanZero(heights)
        auto freeHeights = CountLessThanZero(heights)

        using namespace Common;

        if (freeHeights == 0 && AreEqual(reservedHeight, 1))
            return true;

        if (reservedHeight < 1 - (float) freeHeights * 1.e-2)
            return true;

        auto &log = FLog::Error() << "Inconsistent column widths: ";

        for (auto w: heights) log << (w == -1 ? Str("free") : ToStr(w)) << "; ";

        return false;
    }

    FWindowColumn::FWindowColumn() : FSlabWindow(FSlabWindowConfig{"<col>"})
    {
    }

    void FWindowColumn::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        for (auto &Win: Windows) {
            DrawContentPaneFrame(PlatformWindow, *Win);
            Win->ImmediateDraw(PlatformWindow);
            OpenGL::CheckGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::GetClassName(Win.get()));
        }
    }

    void FWindowColumn::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow) {
        for (auto &Win : Windows) {
            Win->RegisterDeferredDrawCalls(PlatformWindow);
        }
    }

    void FWindowColumn::NotifyReshape(int newWinW, int newWinH) {
        FSlabWindow::NotifyReshape(newWinW, newWinH);

        arrangeWindows();
    }

    bool FWindowColumn::NotifyMouseMotion(int x, int y, int dx, int dy) {
        auto Responded = false;
        for(auto &Win : Windows) if(Win->IsPointWithin(FPoint2D{DevFloat(x) ,DevFloat(y)}) ){
            Responded = Win->NotifyMouseMotion(x,y,dx,dy);
        }

        if(Responded) return true;

        return FSlabWindow::NotifyMouseMotion(x, y, dx, dy);
    }

    bool
    FWindowColumn::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        PropagateEvent(NotifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn)
    }

    bool FWindowColumn::NotifyMouseButton(EMouseButton button, EKeyState state,
                                         EModKeys keys) {
        if(state == Release)
            PropagateEvent(NotifyMouseButton(button, state, keys), AlwaysPropagate)
            // PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn);

        PropagateEvent(NotifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
    }

    bool FWindowColumn::NotifyMouseWheel(double dx, double dy) {
        PropagateEvent(NotifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn)
    }

    bool FWindowColumn::isEmpty() const {
        return Windows.empty();
    }

    void FWindowColumn::Set_x(int x) {
        FSlabWindow::Set_x(x);

        arrangeWindows();
    }

    void FWindowColumn::Set_y(int y) {
        FSlabWindow::Set_y(y);

        arrangeWindows();
    }


}
