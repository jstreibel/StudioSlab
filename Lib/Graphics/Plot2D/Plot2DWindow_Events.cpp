//
// Created by joao on 28/09/23.
//

#include "Plot2DWindow.h"

#include "Graphics/SlabGraphics.h"

#include "Core/Tools/Log.h"
#include "3rdParty/ImGui.h"

namespace Slab {

    bool
    Graphics::Plot2DWindow::NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(Graphics::FSlabWindow::NotifyMouseButton(button, state, keys)) return true;

        static auto time = Timer();

        if (button == MouseButton::MouseButton_RIGHT) {
            if (state == KeyState::Press) {
                time.reset();
            } else if (state == KeyState::Release && time.getElTime_msec() < 200) {
                popupOn = true;

                auto popupName = Str("win_") + title + Str("_popup");
                if (POPUP_ON_MOUSE_CALL) {
                    Core::Log::Info() << "Popup (on mouse call) '" << popupName << "' is on" << Core::Log::Flush;
                    ImGui::OpenPopup(popupName.c_str());
                    popupOn = false;
                }

                Graphics::FSlabWindow::NotifyMouseButton(button, state, keys);
                return true;
            }
        }

        return false;
    }

    bool Graphics::Plot2DWindow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        if(FSlabWindow::NotifyMouseMotion(x, y, dx, dy)) return true;

        bool elRet = false;
        auto mouseState = parent_system_window->GetMouseState();

        if (isMouseLeftClicked()) {
            const auto rect = region.getRect();

            const DevFloat dxClampd = -mouseState->dx / (DevFloat) GetWidth();
            const DevFloat dyClampd = mouseState->dy / (DevFloat) GetHeight();
            const DevFloat wGraph = rect.width();
            const DevFloat hGraph = rect.height();
            const DevFloat dxGraph = wGraph * dxClampd;
            const DevFloat dyGraph = hGraph * dyClampd;

            region = RectR{rect.xMin + dxGraph,
                           rect.xMax + dxGraph,
                           rect.yMin + dyGraph,
                           rect.yMax + dyGraph};

            elRet = true;
        }

        if (isMouseRightClicked()) {
            constexpr const DevFloat factor = 0.01;
            const DevFloat dw = 1 - factor * dx;
            const DevFloat dh = 1 + factor * dy;

            const DevFloat x0 = region.xCenter();
            const DevFloat y0 = region.yCenter();
            const DevFloat hw = .5 * region.width()  * dw;
            const DevFloat hh = .5 * region.height() * dh;

            region = {
                    x0 - hw,
                    x0 + hw,
                    y0 - hh,
                    y0 + hh
            };

            elRet = true;
        }

        return elRet;
    }

    bool Graphics::Plot2DWindow::NotifyMouseWheel(double dx, double dy) {
        if(FSlabWindow::NotifyMouseWheel(dx, dy)) return true;

        constexpr const DevFloat factor = 1.2;
        const DevFloat d = pow(factor, -dy);

        static auto targetRegion = region.getRect();

        // If not animating region at all
        if (!region.isAnimating())
            targetRegion = region.getRect();

        const DevFloat x0 = targetRegion.xCenter();
        const DevFloat hw = .5 * targetRegion.width() * d;

        targetRegion.xMin = x0 - hw;
        targetRegion.xMax = x0 + hw;

        region.animate_xMin(targetRegion.xMin);
        region.animate_xMax(targetRegion.xMax);

        const DevFloat y0 = targetRegion.yCenter();
        const DevFloat hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0 - hh;
        targetRegion.yMax = y0 + hh;

        region.animate_yMin(targetRegion.yMin);
        region.animate_yMax(targetRegion.yMax);

        return true;
    }

    bool Graphics::Plot2DWindow::NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(FSlabWindow::NotifyKeyboard(key, state, modKeys)) return true;

        if(state==KeyState::Release) {
            if(key == KeyMap::Key_TAB && modKeys.Mod_Shift == Press) {
                toggleShowInterface();
                return true;
            }
        }

        return false;
    }

}