//
// Created by joao on 20/04/2021.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/SlabGraphics.h"

#include "SlabWindow.h"
#include "Core/Tools/Log.h"


namespace Slab::Graphics {

    #define USE_GLOBAL_MOUSECLICK_POLICY false

    SlabWindow::SlabWindow(RectI win_rect, Int flags)
            : flags(flags), windowRect(win_rect) {}

    SlabWindow::~SlabWindow() = default;

    void SlabWindow::draw() { setupWindow(); }
    void SlabWindow::notifyReshape(int w, int h) { this->setSize(w, h); }
    bool SlabWindow::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if     (button == MouseButton::MouseButton_LEFT)   mouseLeftButton   = state;
        else if(button == MouseButton::MouseButton_MIDDLE) mouseCenterButton = state;
        else if(button == MouseButton::MouseButton_RIGHT)  mouseRightButton  = state;

        return false;
    }

    bool SlabWindow::notifyMouseMotion(int x, int y) { return false; }
    bool SlabWindow::notifyMouseWheel(double dx, double dy) { return false; }
    bool SlabWindow::notifyKeyboard(Slab::Graphics::KeyMap key, Slab::Graphics::KeyState state,
                                    Slab::Graphics::ModKeys modKeys) { return false; }

    void SlabWindow::setupWindow() const {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        fix vp = getViewport();
        // y_opengl = windowHeight - y_top_left - viewportHeight
        fix x = vp.xMin;
        fix y = parent_systemwindow_h - vp.yMin - vp.height();
        // fix y = vp.yMin;
        fix w = vp.width();
        fix h = vp.height();
        glViewport(x, y, w, h);


    }

    auto SlabWindow::isMouseIn() const -> bool {
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        auto rect = getViewport();

        fix x = rect.xMin;
        fix y = rect.yMin;
        fix w = rect.width();
        fix h = rect.height();

        return mouse.x > x && mouse.x < x + w && mouse.y > y && mouse.y < y + h;
    }

    auto SlabWindow::isMouseLeftClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        return mouse.leftPressed;
#else
        return mouseLeftButton==KeyState::Press;
#endif
    }

    auto SlabWindow::isMouseCenterClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.centerPressed) return false;
#else
        return mouseCenterButton==KeyState::Press;
#endif
    }

    auto SlabWindow::isMouseRightClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.rightPressed) return false;
#else
        return mouseRightButton==KeyState::Press;
#endif
    }

    auto SlabWindow::getMouseViewportCoord() const -> Point2D {
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        auto vpRect = getViewport();

        fix xMouseLocal = mouse.x - vpRect.xMin;
        // fix yMouseLocal = parent_systemwindow_h - mouse.y - vpRect.yMin;
        fix yMouseLocal = mouse.y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }



    void SlabWindow::setDecorate(bool decorate) {
        if(!decorate) flags |=  Flags::NoDecoration;
        else           flags &= ~Flags::NoDecoration;
    }

    void SlabWindow::setClear(bool clear) {
        if(!clear) flags |=  Flags::DontClear;
        else       flags &= ~Flags::DontClear;
    }

    RectI SlabWindow::getViewport() const {
        return windowRect;
    }

    bool SlabWindow::hasMainMenu() const {
        return flags & HasMainMenu;
    }

    bool SlabWindow::isActive() const {
        return isMouseIn();
    }

    Int SlabWindow::getFlags() const {
        return flags;
    }

    bool SlabWindow::wantsFullscreen() const {
        return flags & WantsFullscreen;
    }

    void SlabWindow::setupParentSystemWindowHeight(Int h) {
        parent_systemwindow_h = h;
    }

}