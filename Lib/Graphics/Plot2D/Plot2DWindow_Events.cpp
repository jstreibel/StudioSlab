//
// Created by joao on 28/09/23.
//

#include "Plot2DWindow.h"

#include "Core/Tools/Log.h"
#include "3rdParty/ImGui.h"

namespace Slab {

    bool
    Graphics::FPlot2DWindow::NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) {
        if(Graphics::FSlabWindow::NotifyMouseButton(button, state, keys)) return true;

        static auto time = Timer();

        if (button == EMouseButton::MouseButton_RIGHT) {
            if (state == EKeyState::Press) {
                time.reset();
            } else if (state == EKeyState::Release && time.getElTime_msec() < 200) {
                PopupOn = true;

                auto popupName = Str("win_") + Title + Str("_popup");
                if (POPUP_ON_MOUSE_CALL) {
                    Core::Log::Info() << "Popup (on mouse call) '" << popupName << "' is on" << Core::Log::Flush;
                    ImGui::OpenPopup(popupName.c_str());
                    PopupOn = false;
                }

                Graphics::FSlabWindow::NotifyMouseButton(button, state, keys);
                return true;
            }
        }

        return false;
    }

    bool Graphics::FPlot2DWindow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        if(FSlabWindow::NotifyMouseMotion(x, y, dx, dy)) return true;

        IN ParentWin = w_ParentPlatformWindow.lock();
        if (ParentWin == nullptr) return false;

        bool bReturnValue = false;
        const auto MouseState = ParentWin->GetMouseState();

        if (IsMouseLeftClicked()) {
            const auto rect = Region.getRect();

            const DevFloat dxClampd = -MouseState->dx / (DevFloat) GetWidth();
            const DevFloat dyClampd = MouseState->dy / (DevFloat) GetHeight();
            const DevFloat wGraph = rect.GetWidth();
            const DevFloat hGraph = rect.GetHeight();
            const DevFloat dxGraph = wGraph * dxClampd;
            const DevFloat dyGraph = hGraph * dyClampd;

            Region = RectR{rect.xMin + dxGraph,
                           rect.xMax + dxGraph,
                           rect.yMin + dyGraph,
                           rect.yMax + dyGraph};

            bReturnValue = true;
        }

        if (IsMouseRightClicked()) {
            constexpr const DevFloat factor = 0.01;
            const DevFloat dw = 1 - factor * dx;
            const DevFloat dh = 1 + factor * dy;

            const DevFloat x0 = Region.xCenter();
            const DevFloat y0 = Region.yCenter();
            const DevFloat hw = .5 * Region.width()  * dw;
            const DevFloat hh = .5 * Region.height() * dh;

            Region = {
                    x0 - hw,
                    x0 + hw,
                    y0 - hh,
                    y0 + hh
            };

            bReturnValue = true;
        }

        return bReturnValue;
    }

    bool Graphics::FPlot2DWindow::NotifyMouseWheel(double dx, double dy) {
        if(FSlabWindow::NotifyMouseWheel(dx, dy)) return true;

        constexpr const DevFloat factor = 1.2;
        const DevFloat d = pow(factor, -dy);

        static auto targetRegion = Region.getRect();

        // If not animating region at all
        if (!Region.isAnimating())
            targetRegion = Region.getRect();

        const DevFloat x0 = targetRegion.xCenter();
        const DevFloat hw = .5 * targetRegion.GetWidth() * d;

        targetRegion.xMin = x0 - hw;
        targetRegion.xMax = x0 + hw;

        Region.animate_xMin(targetRegion.xMin);
        Region.animate_xMax(targetRegion.xMax);

        const DevFloat y0 = targetRegion.yCenter();
        const DevFloat hh = .5 * targetRegion.GetHeight() * d;

        targetRegion.yMin = y0 - hh;
        targetRegion.yMax = y0 + hh;

        Region.animate_yMin(targetRegion.yMin);
        Region.animate_yMax(targetRegion.yMax);

        return true;
    }

    bool Graphics::FPlot2DWindow::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        if(FSlabWindow::NotifyKeyboard(key, state, modKeys)) return true;

        if(state==EKeyState::Release) {
            if(key == EKeyMap::Key_TAB && modKeys.Mod_Shift == Press) {
                toggleShowInterface();
                return true;
            }
        }

        return false;
    }

}