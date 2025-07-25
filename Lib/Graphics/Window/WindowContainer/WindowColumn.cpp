//
// Created by joao on 20/08/23.
//

#include "WindowColumn.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/Utils.h"
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

    void WindowColumn::addWindow(const TPointer<FSlabWindow>& window, float windowHeight) {
        Windows.emplace_back(window);
        heights.emplace_back(windowHeight);
    }

    bool WindowColumn::removeWindow(const TPointer<FSlabWindow>& window) {

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

    void WindowColumn::arrangeWindows() {
        if (!assertConsistency()) throw Exception("WindowRow inconsistency");

        auto m = Windows.size();

        if (m == 0) return;

        Vector<int> computedHeights(m, (int) (GetHeight() / m));    // "if(freeHeights==m)"

        auto freeHeights = CountLessThanZero(heights)
        if (freeHeights == 0) {
            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = GetHeight() * relHeight;
                computedHeights[i] = (int) height;
            }
        } else if (freeHeights != m) {
            auto reservedHeight = SumLargerThanZero(heights)
            auto hFree = (float) GetHeight() * (1 - reservedHeight) / (float) freeHeights;

            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = relHeight > 0 ? GetHeight() * relHeight : hFree;
                computedHeights[i] = (int) height;
            }
        }

        Vector<int> computed_yPositions(m);
        auto y = this->Get_y();
        for (int i = 0; i < m; ++i) {
            computed_yPositions[i] = y;
            y += computedHeights[i];
        }

        fix gap = WindowStyle::tiling_gap;
        for (int i = 0; i < m; ++i) {
            computed_yPositions[i] += gap;
            computedHeights[i] -= gap;
        }

        auto i = 0;
        for (auto &win: Windows) {
            win->Set_x(Get_x() + gap);
            win->Set_y(computed_yPositions[i]);

            win->NotifyReshape(GetWidth() - gap, computedHeights[i]);

            i++;
        }
    }

    bool WindowColumn::assertConsistency() const {
        auto reservedHeight = SumLargerThanZero(heights)
        auto freeHeights = CountLessThanZero(heights)

        using namespace Common;

        if (freeHeights == 0 && AreEqual(reservedHeight, 1))
            return true;

        if (reservedHeight < 1 - (float) freeHeights * 1.e-2)
            return true;

        auto &log = Log::Error() << "Inconsistent column widths: ";

        for (auto w: heights) log << (w == -1 ? Str("free") : ToStr(w)) << "; ";

        return false;
    }

    WindowColumn::WindowColumn() : FSlabWindow(FSlabWindowConfig{"<col>"})
    {
    }

    void WindowColumn::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        for (auto &Win: Windows) {
            Win->ImmediateDraw(PlatformWindow);
            OpenGL::CheckGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(Win.get()));
        }
    }

    void WindowColumn::NotifyReshape(int newWinW, int newWinH) {
        FSlabWindow::NotifyReshape(newWinW, newWinH);

        arrangeWindows();
    }

    bool WindowColumn::NotifyMouseMotion(int x, int y, int dx, int dy) {
        auto Responded = false;
        for(auto &Win : Windows) if(Win->IsPointWithin(Point2D{DevFloat(x) ,DevFloat(y)}) ){
            Responded = Win->NotifyMouseMotion(x,y,dx,dy);
        }

        if(Responded) return true;

        return FSlabWindow::NotifyMouseMotion(x, y, dx, dy);
    }

    bool
    WindowColumn::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        PropagateEvent(NotifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn)
    }

    bool WindowColumn::NotifyMouseButton(EMouseButton button, EKeyState state,
                                         EModKeys keys) {
        if(state == Release)
            PropagateEvent(NotifyMouseButton(button, state, keys), AlwaysPropagate)
            // PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn);

        PropagateEvent(NotifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
    }

    bool WindowColumn::NotifyMouseWheel(double dx, double dy) {
        PropagateEvent(NotifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn)
    }

    bool WindowColumn::isEmpty() const {
        return Windows.empty();
    }

    void WindowColumn::Set_x(int x) {
        FSlabWindow::Set_x(x);

        arrangeWindows();
    }

    void WindowColumn::Set_y(int y) {
        FSlabWindow::Set_y(y);

        arrangeWindows();
    }


}