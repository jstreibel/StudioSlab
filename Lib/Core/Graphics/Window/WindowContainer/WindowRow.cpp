//
// Created by joao on 20/08/23.
//

#include "WindowRow.h"
#include "Core/Tools/Log.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Core/Backend/BackendManager.h"

#include <algorithm>
#include <numeric>

#define CountLessThanZero(vec) std::count_if(begin(vec), end(vec), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(begin(vec), end(vec), 0.0f, [](float acc, float x) { return x > 0 ? acc + x : acc; });

#define PropagateEvent(EVENT)   \
    auto responded = false;     \
    for(auto &winData : windowsList)    \
        if(winData.window->isMouseIn()) responded = winData.window->EVENT; \
                                \
    return responded;


namespace Graphics {

    WindowRow::WindowRow(Window::Flags flags)
            : Window(0, 0, 100, 100, flags) {

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

    bool WindowRow::addWindow(const Window::Ptr &window, RelativePosition relPosition,
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

    void WindowRow::removeWindow(const Window::Ptr &window) {
        windowsList.remove_if([&window](WinMetaData &toComp) {
            return toComp.window == window;
        });

        arrangeWindows();
    }

    void WindowRow::arrangeWindows() {
        if (!assertConsistency()) throw Exception("WindowRow inconsistency");

        auto m = windowsList.size();

        if (m == 0) return;

        std::vector<int> computedWidths(m, (int) (getw() / m));    // "if(freeWidths==m)"
        auto widths = _widthsVector();

        auto freeWidths = CountLessThanZero(widths);
        if (freeWidths == 0) {
            for (int i = 0; i < m; ++i) {
                auto relWidth = widths[i];
                auto width = geth() * relWidth;
                computedWidths[i] = (int) width;
            }
        } else if (freeWidths != m) {
            auto reservedWidth = SumLargerThanZero(widths);
            auto wFree = (float) getw() * (1 - reservedWidth) / (float) freeWidths;

            for (int i = 0; i < m; ++i) {
                auto relWidth = widths[i];
                auto width = relWidth > 0 ? getw() * relWidth : wFree;
                computedWidths[i] = (int) width;
            }
        }

        std::vector<int> computed_xPositions(m);
        auto x = this->getx();
        for (int i = 0; i < m; ++i) {
            computed_xPositions[i] = x;
            x += computedWidths[i];
        }

        auto i = 0;
        int menuRoom = flags & HasMainMenu ? Core::Graphics::menuHeight : 0;
        for (auto &winMData: windowsList) {
            OUT win = *winMData.window;

            win.setx(computed_xPositions[i]);
            win.sety(gety());

            win.notifyReshape(computedWidths[i], geth() - menuRoom);

            i++;
        }
    }

    bool WindowRow::assertConsistency() const {
        auto widths = _widthsVector();

        auto reserverdWidth = SumLargerThanZero(widths);
        auto freeWidths = CountLessThanZero(widths);

        using namespace Common;

        if (freeWidths == 0 && areEqual(reserverdWidth, 1))
            return true;

        if (reserverdWidth < 1 - (float) freeWidths * 1.e-2)
            return true;

        auto &log = Log::Error() << "Inconsistent column widths: ";

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
        Window::notifyReshape(w, h);

        arrangeWindows();
    }

    bool WindowRow::notifyMouseMotion(int x, int y) {
        for (auto &winData: windowsList)
            if (winData.window->isMouseIn() && winData.window->notifyMouseMotion(x, y)) return true;

        //auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();
        //if(mouseState.leftPressed){
        //    Log::Error() << "Resize not implemented" << Log::Flush;
        //    for(auto &winData : windowsList) {
//
        //    }
        //}

        return false;
    }

    bool WindowRow::notifyMouseButton(Core::MouseButton button, Core::KeyState state,
                                      Core::ModKeys keys) {
        PropagateEvent(notifyMouseButton(button, state, keys));
    }

    bool WindowRow::notifyMouseWheel(double dx, double dy) {
        PropagateEvent(notifyMouseWheel(dx, dy));
    }

    bool WindowRow::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
        PropagateEvent(notifyKeyboard(key, state, modKeys));
    }

    bool WindowRow::notifyFilesDropped(StrVector paths) {
        PropagateEvent(notifyFilesDropped(paths));
    }

}