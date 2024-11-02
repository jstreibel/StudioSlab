//
// Created by joao on 20/08/23.
//

#include "WindowRow.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/Utils.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Window/WindowStyles.h"

#include <algorithm>
#include <numeric>

#define CountLessThanZero(vec) std::count_if(begin(vec), end(vec), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(begin(vec), end(vec), 0.0f, [](float acc, float x) { return x > 0 ? acc + x : acc; });



// Don't touch
fix AlwaysPropagate = true;
fix PropagateOnlyIfMouseIsIn = false;

#define PropagateEvent(EVENT, PROPAGATE_ALWAYS) \
    {                                            \
    auto responded = false;     \
    for(auto &winData : windowsList)    \
        if(winData.window->isMouseIn() || PROPAGATE_ALWAYS) responded = winData.window->EVENT; \
                                \
    return responded;                           \
    }


namespace Slab::Graphics {

    WindowRow::WindowRow(Str title, Int flags)
            : SlabWindow({title, WindowStyle::default_window_rect, flags}) {

    }

    RealVector WindowRow::_widthsVector() const {
        auto widths = RealVector(windowsList.size());

        auto i = 0;
        for (auto &winData: windowsList) {
            widths[i] = winData.width;
            ++i;
        }

        return widths;
    }

    bool WindowRow::addWindow(const Pointer<SlabWindow> &window, RelativePosition relPosition,
                              float windowWidth) {
        if (std::find_if(windowsList.begin(), windowsList.end(),
                         [&window](WinMetaData &winMetaData) {
                             return winMetaData.window == window;
                         }) != windowsList.end()) {
            return false;
        }

        switch (relPosition) {
            case Left:
                windowsList.push_front({window, windowWidth});
                arrangeWindows();
                return true;
            case Right:
                windowsList.push_back({window, windowWidth});
                arrangeWindows();
                return true;
        }

        return false;
    }

    void WindowRow::removeWindow(const Pointer<SlabWindow> &window) {
        windowsList.remove_if([&window](WinMetaData &toComp) {
            return toComp.window == window;
        });

        arrangeWindows();
    }

    void WindowRow::arrangeWindows() {
        if (!assertConsistency()) throw Exception("WindowRow inconsistency");

        auto m = windowsList.size();

        if (m == 0) return;

        Vector<int> computedWidths(m);
        auto widths = _widthsVector();
        auto freeWidths = CountLessThanZero(widths);

        if(freeWidths == m) {
            computedWidths = Vector<int>(m, (int) (getw() / m));
        }
        else if (freeWidths == 0) {
            for (int i = 0; i < m; ++i) {
                auto relWidth = widths[i];
                auto width = geth() * relWidth - 2*WindowStyle::tiling_gap;
                computedWidths[i] = (int) width;
            }
        } else /*if (freeWidths != m)*/ {
            auto reservedWidth = SumLargerThanZero(widths);
            auto wFree = (float) getw() * (1 - reservedWidth) / (float) freeWidths;

            for (int i = 0; i < m; ++i) {
                auto relWidth = widths[i];
                auto width = relWidth > 0 ? getw() * relWidth : wFree;
                computedWidths[i] = (int) width;
            }
        }

        Vector<int> computed_xPositions(m);
        {
            auto x = this->getx();
            for (int i = 0; i < m; ++i) {
                computed_xPositions[i] = x;
                x += computedWidths[i];
            }

            for (int i = 0; i < m; ++i) {
                computed_xPositions[i] += WindowStyle::tiling_gap;
                computedWidths[i]      -= WindowStyle::tiling_gap;
            }
        }

        auto i = 0;
        fix y = gety() + WindowStyle::tiling_gap;
        fix h = geth() - WindowStyle::tiling_gap;
        for (auto &winMData: windowsList) {
            OUT win = *winMData.window;

            win.setx(computed_xPositions[i]);
            win.sety(y);

            win.notifyReshape(computedWidths[i], h);

            i++;
        }
    }

    bool WindowRow::assertConsistency() const {
        auto widths = _widthsVector();

        auto reserverdWidth = SumLargerThanZero(widths);
        auto freeWidths = CountLessThanZero(widths);

        using namespace Common;

        if (freeWidths == 0 && AreEqual(reserverdWidth, 1))
            return true;

        if (reserverdWidth < 1 - (float) freeWidths * 1.e-2)
            return true;

        auto &log = Core::Log::Error() << "Inconsistent column widths: ";

        for (auto w: widths) log << (w == -1 ? Str("free") : ToStr(w)) << "; ";

        return false;
    }

    void WindowRow::draw() {
        for (auto &winData: windowsList) {
            auto &window = *winData.window;

            window.draw();
            OpenGL::checkGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(&window));
        }
    }

    void WindowRow::notifyReshape(int w, int h) {
        SlabWindow::notifyReshape(w, h);

        arrangeWindows();
    }

    bool WindowRow::notifyMouseMotion(int x, int y) {
        for (auto &winData: windowsList)
            if (winData.window->isMouseIn() && winData.window->notifyMouseMotion(x, y)) return true;

        //auto mouseState = Slab::Graphics::GetGraphicsBackend()->getMouseState();
        //if(mouseState.leftPressed){
        //    Log::Error() << "Resize not implemented" << Log::Flush;
        //    for(auto &winData : windowsList) {
//
        //    }
        //}

        return false;
    }

    bool WindowRow::notifyMouseButton(MouseButton button, KeyState state,
                                      ModKeys keys) {
        if(state == KeyState::Release)
            // PropagateEvent(notifyMouseButton(button, state, keys), AlwaysPropagate)
            PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
        else
            PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
    }

    bool WindowRow::notifyMouseWheel(double dx, double dy) {
        PropagateEvent(notifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn)
    }

    bool WindowRow::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        PropagateEvent(notifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn)
    }

    void WindowRow::setx(int x) {
        SlabWindow::setx(x);

        arrangeWindows();
    }

    void WindowRow::sety(int y) {
        SlabWindow::sety(y);

        arrangeWindows();
    }

}