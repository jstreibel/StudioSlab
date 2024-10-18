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


#define PropagateEvent(EVENT, PROPAGATE_ALWAYS){   \
    auto responded = false;     \
    for(auto &win : windows)    \
        if(win->isMouseIn() || PROPAGATE_ALWAYS) responded = win->EVENT; \
                                \
    return responded;}


namespace Slab::Graphics {

    using namespace Core;

    void WindowColumn::addWindow(Window_ptr window, float windowHeight) {
        windows.emplace_back(window);
        heights.emplace_back(windowHeight);
    }

    bool WindowColumn::removeWindow(const Window_ptr& window) {

        auto it = windows.begin();
        for(auto i=0; i<windows.size(); ++i){
            if(*it == window) {
                windows.remove(*it);
                heights.erase(heights.begin()+i);

                return true;
            }

            it++;
        }

        return false;
    }

    void WindowColumn::arrangeWindows() {
        if (!assertConsistency()) throw "WindowRow inconsistency";

        auto m = windows.size();

        if (m == 0) return;

        Vector<int> computedHeights(m, (int) (geth() / m));    // "if(freeHeights==m)"

        auto freeHeights = CountLessThanZero(heights);
        if (freeHeights == 0) {
            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = geth() * relHeight;
                computedHeights[i] = (int) height;
            }
        } else if (freeHeights != m) {
            auto reservedHeight = SumLargerThanZero(heights);
            auto hFree = (float) geth() * (1 - reservedHeight) / (float) freeHeights;

            for (int i = 0; i < m; ++i) {
                auto relHeight = heights[i];
                auto height = relHeight > 0 ? geth() * relHeight : hFree;
                computedHeights[i] = (int) height;
            }
        }

        Vector<int> computed_yPositions(m);
        auto y = this->gety();
        for (int i = 0; i < m; ++i) {
            computed_yPositions[i] = y;
            y += computedHeights[i];
        }

        auto i = 0;
        for (auto &win: windows) {
            win->setx(getx());
            win->sety(computed_yPositions[i]);

            win->notifyReshape(getw(), computedHeights[i]);

            i++;
        }
    }

    bool WindowColumn::assertConsistency() const {
        auto reservedHeight = SumLargerThanZero(heights);
        auto freeHeights = CountLessThanZero(heights);

        using namespace Common;

        if (freeHeights == 0 && AreEqual(reservedHeight, 1))
            return true;

        if (reservedHeight < 1 - (float) freeHeights * 1.e-2)
            return true;

        auto &log = Log::Error() << "Inconsistent column widths: ";

        for (auto w: heights) log << (w == -1 ? Str("free") : ToStr(w)) << "; ";

        return false;
    }

    void WindowColumn::draw() {
        for (auto &win: windows) {
            win->draw();
            OpenGL::checkGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(win.get()));
        }
    }

    void WindowColumn::notifyReshape(int newWinW, int newWinH) {
        Window::notifyReshape(newWinW, newWinH);

        arrangeWindows();
    }

    bool WindowColumn::notifyMouseMotion(int x, int y) {
        auto responded = false;
        for(auto &win : windows)
        if(win->isMouseIn() ){
            responded = win->notifyMouseMotion(x,y);
        }

        if(!responded) {

        }

        return responded;
    }

    bool
    WindowColumn::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        PropagateEvent(notifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn);
    }

    bool WindowColumn::notifyMouseButton(MouseButton button, KeyState state,
                                         ModKeys keys) {
        if(state == Release)
            // PropagateEvent(notifyMouseButton(button, state, keys), AlwaysPropagate);
            PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn);

        PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn);
    }

    bool WindowColumn::notifyMouseWheel(double dx, double dy) {
        PropagateEvent(notifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn);
    }

    bool WindowColumn::notifyFilesDropped(StrVector paths) {
        PropagateEvent(notifyFilesDropped(paths), PropagateOnlyIfMouseIsIn);
    }

    bool WindowColumn::isEmpty() const {
        return windows.empty();
    }


}