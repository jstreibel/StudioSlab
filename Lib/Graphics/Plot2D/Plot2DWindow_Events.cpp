//
// Created by joao on 28/09/23.
//

#include "Plot2DWindow.h"

#include "Graphics/SlabGraphics.h"

#include "Core/Tools/Log.h"
#include "3rdParty/ImGui.h"

namespace Slab {

    bool
    Graphics::Plot2DWindow::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(Graphics::SlabWindow::notifyMouseButton(button, state, keys)) return true;

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

                Graphics::SlabWindow::notifyMouseButton(button, state, keys);
                return true;
            }
        }

        return false;
    }

    bool Graphics::Plot2DWindow::notifyMouseMotion(int x, int y, int dx, int dy) {
        if(SlabWindow::notifyMouseMotion(x, y, dx, dy)) return true;

        bool elRet = false;
        auto mouseState = parent_system_window->getMouseState();

        if (isMouseLeftClicked()) {
            const auto rect = region.getRect();

            const Real dxClampd = -mouseState->dx / (Real) GetWidth();
            const Real dyClampd = mouseState->dy / (Real) GetHeight();
            const Real wGraph = rect.width();
            const Real hGraph = rect.height();
            const Real dxGraph = wGraph * dxClampd;
            const Real dyGraph = hGraph * dyClampd;

            region = RectR{rect.xMin + dxGraph,
                           rect.xMax + dxGraph,
                           rect.yMin + dyGraph,
                           rect.yMax + dyGraph};

            elRet = true;
        }

        if (isMouseRightClicked()) {
            constexpr const Real factor = 0.01;
            const Real dw = 1 - factor * dx;
            const Real dh = 1 + factor * dy;

            const Real x0 = region.xCenter();
            const Real y0 = region.yCenter();
            const Real hw = .5 * region.width()  * dw;
            const Real hh = .5 * region.height() * dh;

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

    bool Graphics::Plot2DWindow::notifyMouseWheel(double dx, double dy) {
        if(SlabWindow::notifyMouseWheel(dx, dy)) return true;

        constexpr const Real factor = 1.2;
        const Real d = pow(factor, -dy);

        static auto targetRegion = region.getRect();

        // If not animating region at all
        if (!region.isAnimating())
            targetRegion = region.getRect();

        const Real x0 = targetRegion.xCenter();
        const Real hw = .5 * targetRegion.width() * d;

        targetRegion.xMin = x0 - hw;
        targetRegion.xMax = x0 + hw;

        region.animate_xMin(targetRegion.xMin);
        region.animate_xMax(targetRegion.xMax);

        const Real y0 = targetRegion.yCenter();
        const Real hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0 - hh;
        targetRegion.yMax = y0 + hh;

        region.animate_yMin(targetRegion.yMin);
        region.animate_yMax(targetRegion.yMax);

        return true;
    }

    bool Graphics::Plot2DWindow::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(SlabWindow::notifyKeyboard(key, state, modKeys)) return true;

        if(state==KeyState::Release) {
            if(key == KeyMap::Key_TAB && modKeys.Mod_Shift == Press) {
                toggleShowInterface();
                return true;
            }
        }

        return false;
    }

}