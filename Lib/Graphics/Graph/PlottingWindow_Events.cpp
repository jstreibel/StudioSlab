//
// Created by joao on 28/09/23.
//

#include "PlottingWindow.h"
#include "Core/Tools/Log.h"
#include "3rdParty/ImGui.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/Animator/Animator.h"

#define POPUP_ON_MOUSE_CALL false


namespace Slab {

    bool
    Graphics::PlottingWindow::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
        static auto time = Timer();

        if (button == Core::MouseButton_RIGHT) {
            if (state == Core::Press) {
                time.reset();
            } else if (state == Core::Release && time.getElTime_msec() < 200) {
                popupOn = true;

                auto popupName = Str("win_") + title + Str("_popup");
                if (POPUP_ON_MOUSE_CALL) {
                    Core::Log::Info() << "Popup (on mouse call) '" << popupName << "' is on" << Core::Log::Flush;
                    ImGui::OpenPopup(popupName.c_str());
                    popupOn = false;
                }

                Graphics::Window::notifyMouseButton(button, state, keys);
                return true;
            }
        }

        return Graphics::Window::notifyMouseButton(button, state, keys);
    }

    bool Graphics::PlottingWindow::notifyMouseMotion(int x, int y) {
        auto elRet = GUIEventListener::notifyMouseMotion(x, y);

        auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

        if (isMouseLeftClicked()) {
            const auto rect = region.getRect();

            const Real dxClampd = -mouseState.dx / (Real) getw();
            const Real dyClampd = mouseState.dy / (Real) geth();
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
            const Real dx = 1 - factor * mouseState.dx;
            const Real dy = 1 + factor * mouseState.dy;

            const Real x0 = region.xCenter();
            const Real y0 = region.yCenter();
            const Real hw = .5 * region.width() * dx;
            const Real hh = .5 * region.height() * dy;

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

    bool Graphics::PlottingWindow::notifyMouseWheel(double dx, double dy) {
        GUIEventListener::notifyMouseWheel(dx, dy);

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

    void Graphics::PlottingWindow::notifyReshape(int newWinW, int newWinH) { Window::notifyReshape(newWinW, newWinH); }

    bool Graphics::PlottingWindow::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
        if(state==Core::Release) {
            if(key == Core::Key_TAB) {
                toggleShowInterface();
                return true;
            }
        }

        return GUIEventListener::notifyKeyboard(key, state, modKeys);
    }

}