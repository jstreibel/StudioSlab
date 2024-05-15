//
// Created by joao on 28/09/23.
//

#include "Graph.h"
#include "Core/Tools/Log.h"
#include "imgui.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Animator.h"

#define POPUP_ON_MOUSE_CALL false

bool Graphics::Graph2D::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
    static auto time = Timer();

    if(button == Core::MouseButton_RIGHT){
        if(state == Core::Press) time.reset();
        else if(state == Core::Release && time.getElTime_msec() < 200){
            savePopupOn = true;

            auto popupName = Str("win_") + title + Str("_popup");
            if(POPUP_ON_MOUSE_CALL) {
                Log::Info() << "Popup (on mouse call) '" << popupName << "' is on" << Log::Flush;
                ImGui::OpenPopup(popupName.c_str());
                savePopupOn = false;
            }
        }
    }

    return Graphics::Window::notifyMouseButton(button, state, keys);
}

bool Graphics::Graph2D::notifyMouseMotion(int x, int y) {
    auto elRet = GUIEventListener::notifyMouseMotion(x, y);

    auto mouseState = Core::BackendManager::GetGUIBackend().getMouseState();

    if(isMouseLeftClicked())
    {
        const Real dxClampd = -mouseState.dx / (Real)getw();
        const Real dyClampd = mouseState.dy / (Real)geth();
        const Real wGraph = region.width();
        const Real hGraph = region.height();
        const Real dxGraph = wGraph * dxClampd;
        const Real dyGraph = hGraph * dyClampd;

        region.xMin += dxGraph;
        region.xMax += dxGraph;
        region.yMin += dyGraph;
        region.yMax += dyGraph;
    }
    if(isMouseRightClicked())
    {
        constexpr const Real factor = 0.01;
        const Real dx = 1-factor*mouseState.dx;
        const Real dy = 1+factor*mouseState.dy;

        const Real x0 = region.xCenter();
        const Real y0 = region.yCenter();
        const Real hw = .5 * region.width() *dx;
        const Real hh = .5 * region.height()*dy;

        region = {
                x0 - hw,
                x0 + hw,
                y0 - hh,
                y0 + hh
        };
    }

    return elRet;
}

bool Graphics::Graph2D::notifyMouseWheel(double dx, double dy) {
    GUIEventListener::notifyMouseWheel(dx, dy);

    constexpr const Real factor = 1.2;
    const Real d = pow(factor, -dy);

    static auto targetRegion = region;

    if(!Core::Animator::Contains(region.xMin)
       && !Core::Animator::Contains(region.xMax)
       && !Core::Animator::Contains(region.yMin)
       && !Core::Animator::Contains(region.yMax)) {
        targetRegion = region;
    }

    if(Core::BackendManager::GetGUIBackend().getMouseState().rightPressed) {

        const Real x0 = targetRegion.xCenter();
        const Real hw = .5*targetRegion.width() * d;

        targetRegion.xMin = x0-hw;
        targetRegion.xMax = x0+hw;

        set_xMin(targetRegion.xMin);
        set_xMax(targetRegion.xMax);

    } else {
        const Real y0 = targetRegion.yCenter();
        const Real hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0-hh;
        targetRegion.yMax = y0+hh;

        set_yMin(targetRegion.yMin);
        set_yMax(targetRegion.yMax);
    }

    return true;
}

void Graphics::Graph2D::notifyReshape(int newWinW, int newWinH) { Window::notifyReshape(newWinW, newWinH); }