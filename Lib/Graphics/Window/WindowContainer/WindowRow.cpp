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
    auto Responded = false;                                                                \
                                                                                           \
    Responded = FSlabWindow::EVENT;                                                         \
                                                                                           \
    for(auto &WinData : WindowsList)                                                       \
        if(WinData.window->IsMouseInside() || PROPAGATE_ALWAYS) Responded = WinData.window->EVENT; \
                                                                                           \
    return Responded;                                                                      \
}


namespace Slab::Graphics {

    FWindowRow::FWindowRow(Str Title, Int Flags)
            : FSlabWindow(FSlabWindowConfig{std::move(Title), WindowStyle::DefaultWindowRect, Flags}) {

    }

    RealVector FWindowRow::_widthsVector() const {
        auto widths = RealVector(WindowsList.size());

        auto i = 0;
        for (auto &winData: WindowsList) {
            widths[i] = winData.width;
            ++i;
        }

        return widths;
    }

    bool FWindowRow::AddWindow(const Pointer<FSlabWindow> &window, RelativePosition relPosition,
                              float windowWidth) {
        if (std::ranges::find_if(WindowsList,
                                 [&window](WinMetaData &winMetaData) {
                                     return winMetaData.window == window;
                                 }) != WindowsList.end()) {
            return false;
        }

        switch (relPosition) {
            case Left:
                WindowsList.push_front({window, windowWidth});
                arrangeWindows();
                return true;
            case Right:
                WindowsList.push_back({window, windowWidth});
                arrangeWindows();
                return true;
        }

        return false;
    }

    void FWindowRow::removeWindow(const Pointer<FSlabWindow> &window) {
        WindowsList.remove_if([&window](WinMetaData &toComp) {
            return toComp.window == window;
        });

        arrangeWindows();
    }

    void FWindowRow::arrangeWindows() {
        if (!assertConsistency()) throw Exception("WindowRow inconsistency");

        auto m = WindowsList.size();

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
            auto x = this->Get_x();
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
        fix y = Get_y() + WindowStyle::tiling_gap;
        fix h = GetHeight() - WindowStyle::tiling_gap;
        for (auto &winMData: WindowsList) {
            OUT win = *winMData.window;

            win.Set_x(computed_xPositions[i]);
            win.Set_y(y);

            win.NotifyReshape(computedWidths[i], h);

            i++;
        }
    }

    bool FWindowRow::assertConsistency() const {
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

    void FWindowRow::ImmediateDraw(const FPlatformWindow& PlatformWindow) {

        for (auto & winData : std::ranges::reverse_view(WindowsList)) {
            auto &window = *winData.window;

            window.ImmediateDraw(PlatformWindow);
            OpenGL::CheckGLErrors(
                    Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(&window));
        }
    }

    void FWindowRow::NotifyReshape(int w, int h)  {
        FSlabWindow::NotifyReshape(w, h);

        arrangeWindows();
    }

    bool FWindowRow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        for (auto & [Window, Width]: WindowsList)
        {
            if (Window->IsMouseInside() && Window->NotifyMouseMotion(x, y, dx, dy))
                return true;
        }

        //auto mouseState = Slab::Graphics::GetGraphicsBackend()->getMouseState();
        //if(mouseState.leftPressed){
        //    Log::Error() << "Resize not implemented" << Log::Flush;
        //    for(auto &winData : windowsList) {
//
        //    }
        //}

        return false;
    }

    bool FWindowRow::NotifyMouseButton(EMouseButton button, EKeyState state,
                                      EModKeys keys) {
        if(state == EKeyState::Release)
            PropagateEvent(NotifyMouseButton(button, state, keys), AlwaysPropagate)
            // PropagateEvent(notifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
        else
            PropagateEvent(NotifyMouseButton(button, state, keys), PropagateOnlyIfMouseIsIn)
    }

    bool FWindowRow::NotifyMouseWheel(double dx, double dy) {
        PropagateEvent(NotifyMouseWheel(dx, dy), PropagateOnlyIfMouseIsIn)
    }

    bool FWindowRow::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        PropagateEvent(NotifyKeyboard(key, state, modKeys), PropagateOnlyIfMouseIsIn)
    }

    void FWindowRow::Set_x(int x) {
        FSlabWindow::Set_x(x);

        arrangeWindows();
    }

    void FWindowRow::Set_y(int y) {
        FSlabWindow::Set_y(y);

        arrangeWindows();
    }

}