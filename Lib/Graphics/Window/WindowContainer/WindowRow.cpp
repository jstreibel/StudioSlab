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
#include <ranges>
#include <utility>

#define CountLessThanZero(vec) std::count_if(begin(vec), end(vec), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(begin(vec), end(vec), 0.0f, [this](float acc, float x) { return x > 0 ? acc + (x<=1?x:x/GetWidth()) : acc; });

// Don't touch
fix AlwaysPropagate = true;
fix PropagateOnlyIfMouseIsIn = false;

#define PropagateEvent(EVENT, PROPAGATE_ALWAYS){                                           \
    auto responded = false;                                                                \
                                                                                           \
    responded = FSlabWindow::EVENT;                                                         \
                                                                                           \
    for(auto &winData : windowsList)                                                       \
        if(winData.window->isMouseIn() || PROPAGATE_ALWAYS) responded = winData.window->EVENT; \
                                                                                           \
    return responded;                                                                      \
}


namespace Slab::Graphics {

    WindowRow::WindowRow(Str title, Int flags)
            : FSlabWindow({std::move(title), WindowStyle::default_window_rect, flags}) {

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

    bool WindowRow::addWindow(const Pointer<FSlabWindow> &window, RelativePosition relPosition,
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

    void WindowRow::removeWindow(const Pointer<FSlabWindow> &window) {
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
        auto freeWidths = CountLessThanZero(widths)

        if(freeWidths == m) {
            computedWidths = Vector<int>(m, (int) (GetWidth() / m));
        }
        else if (freeWidths == 0) {
            for (int i = 0; i < m; ++i) {
                auto input_width = widths[i];
                int computed_width = input_width < 1. ? int(GetHeight() * computed_width) : int(computed_width);
                computedWidths[i] = (int) computed_width;
            }
        } else /*if (freeWidths != m)*/ {
            auto reservedWidth = SumLargerThanZero(widths)
            auto wFree = (float) GetWidth() * (1 - reservedWidth) / (float) freeWidths;

            for (int i = 0; i < m; ++i) {
                auto input_width = widths[i];
                int computed_width;
                if(input_width > 0) {
                    if(input_width <= 1)
                        computed_width = (int)(GetWidth() * input_width);
                    else
                        computed_width = (int)input_width;
                } else
                    computed_width = (int)wFree;

                computedWidths[i] = (int) computed_width;
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
        fix h = GetHeight() - WindowStyle::tiling_gap;
        for (auto &winMData: windowsList) {
            OUT win = *winMData.window;

            win.Set_x(computed_xPositions[i]);
            win.Set_y(y);

            win.NotifyReshape(computedWidths[i], h);

            i++;
        }
    }

    bool WindowRow::assertConsistency() const {
        auto widths = _widthsVector();

        auto reserverdWidth = SumLargerThanZero(widths)
        auto freeWidths = CountLessThanZero(widths)

        using namespace Common;

        if (freeWidths == 0 && AreEqual(reserverdWidth, 1))
            return true;

        if (reserverdWidth < 1 - (float) freeWidths * 1.e-2)
            return true;

        auto &log = Core::Log::Error() << "Inconsistent column widths: ";

        for (auto w: widths) log << (w == -1 ? Str("free") : ToStr(w)) << "; ";

        return false;
    }

    void WindowRow::Draw() {

        for (auto & winData : std::ranges::reverse_view(windowsList)) {
            auto &window = *winData.window;

            window.Draw();
            OpenGL::checkGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(&window));
        }
    }

    void WindowRow::NotifyReshape(int w, int h)  {
        FSlabWindow::NotifyReshape(w, h);

        arrangeWindows();
    }

    bool WindowRow::notifyMouseMotion(int x, int y, int dx, int dy) {
        for (auto &winData: windowsList)
            if (winData.window->isMouseIn() && winData.window->notifyMouseMotion(x, y, dx, dy))
                return true;

        //auto mouseState = Slab::Graphics::GetGraphicsBackend()->getMouseState();
        //if(mouseState.leftPressed){
        //    Log::Error() << "Resize not implemented" << Log::Flush;
        //    for(auto &winData : windowsList) {
//
        //    }
        //}

        return false;
    }

    bool WindowRow::NotifyMouseButton(MouseButton button, KeyState state,
                                      ModKeys keys) {
        if(state == KeyState::Release)
            PropagateEvent(NotifyMouseButton(button, state, keys), AlwaysPropagate)
            // PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
        else
            PropagateEvent(NotifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
    }

    bool WindowRow::notifyMouseWheel(double dx, double dy) {
        PropagateEvent(notifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn)
    }

    bool WindowRow::NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        PropagateEvent(NotifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn)
    }

    void WindowRow::Set_x(int x) {
        FSlabWindow::Set_x(x);

        arrangeWindows();
    }

    void WindowRow::Set_y(int y) {
        FSlabWindow::Set_y(y);

        arrangeWindows();
    }

}