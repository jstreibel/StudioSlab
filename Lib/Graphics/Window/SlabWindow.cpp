//
// Created by joao on 20/04/2021.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/SlabGraphics.h"

#include "SlabWindow.h"

#include <utility>
#include "Core/Tools/Log.h"


namespace Slab::Graphics {

    #define USE_GLOBAL_MOUSECLICK_POLICY false

    SlabWindow::
    SlabWindow(Config cfg, const Pointer<SystemWindow>& parent_syswin)
    : config(std::move(cfg))
    , parent_system_window(parent_syswin)
    {
        if(config.title.empty()){
            config.title = "[Window:" + ToStr(get_id()) + "]";
        }

        if(parent_system_window == nullptr)
            parent_system_window = GetGraphicsBackend()->GetMainSystemWindow();
    }

    SlabWindow::~SlabWindow() = default;

    void SlabWindow::draw() { setupWindow(); }
    void SlabWindow::notifyReshape(int w, int h) {
        config.win_rect.xMax = config.win_rect.xMin + w;
        config.win_rect.yMax = config.win_rect.yMin + h;
    }
    bool SlabWindow::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if     (button == MouseButton::MouseButton_LEFT)   mouseLeftButton   = state;
        else if(button == MouseButton::MouseButton_MIDDLE) mouseCenterButton = state;
        else if(button == MouseButton::MouseButton_RIGHT)  mouseRightButton  = state;

        return false;
    }

    bool SlabWindow::notifyMouseMotion(int x, int y, int dx, int dy) { return false; }
    bool SlabWindow::notifyMouseWheel(double dx, double dy) { return false; }
    bool SlabWindow::notifyKeyboard(Slab::Graphics::KeyMap key, Slab::Graphics::KeyState state,
                                    Slab::Graphics::ModKeys modKeys) { return false; }

    void SlabWindow::setupWindow() const {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        fix syswin_h = parent_system_window->getHeight();
        fix vp = getViewport();
        //// y_opengl = windowHeight - y_top_left - viewportHeight
        fix x = vp.xMin;
        fix y = syswin_h - vp.yMin - vp.height();
        //// fix y = vp.yMin;
        fix w = vp.width();
        fix h = vp.height();
        glViewport(x, y, w, h);

    }

    auto SlabWindow::isMouseIn() const -> bool {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = parent_system_window->getMouseState();

        auto rect = getViewport();

        fix x = rect.xMin;
        fix y = rect.yMin;
        fix w = rect.width();
        fix h = rect.height();

        return mouse->x > x && mouse->x < x + w && mouse->y > y && mouse->y < y + h;
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
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = parent_system_window->getMouseState();
        auto vpRect = getViewport();

        fix xMouseLocal = mouse->x - vpRect.xMin;
        // fix yMouseLocal = parent_systemwindow_h - mouse.y - vpRect.yMin;
        fix yMouseLocal = mouse->y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }



    void SlabWindow::setDecorate(bool decorate) {
        if(!decorate)  config.flags |=  Flags::NoDecoration;
        else           config.flags &= ~Flags::NoDecoration;
    }

    void SlabWindow::setClear(bool clear) {
        if(!clear) config.flags |=  Flags::DontClear;
        else       config.flags &= ~Flags::DontClear;
    }

    RectI SlabWindow::getViewport() const {
        return config.win_rect;
    }

    Str SlabWindow::getTitle() const { return config.title; }

    Int SlabWindow::getFlags() const {
        return config.flags;
    }

    bool SlabWindow::wantsFullscreen() const {
        return config.flags & WantsFullscreen;
    }

    void SlabWindow::SetMinimumWidth(Slab::Resolution minw) {
        min_width = minw;

        if(min_width > GetWidth()) notifyReshape((int)min_width, GetHeight());
    }

    void SlabWindow::SetMinimumHeight(Slab::Resolution minh) {
        min_height = minh;

        if(min_height > GetHeight()) notifyReshape(GetWidth(), (int)min_height);
    }

    auto SlabWindow::setx(int x) -> void {
        fix w=config.win_rect.width();
        config.win_rect.xMin = x;
        config.win_rect.xMax = x+w;
    }

    auto SlabWindow::sety(int y) -> void {
        fix h=config.win_rect.height();
        config.win_rect.yMin = y;
        config.win_rect.yMax = y+h;
    }

    void SlabWindow::notifyBecameActive() { active = true; }

    void SlabWindow::notifyBecameInactive() { active = false; }

    bool SlabWindow::isActive() const { return active; }




}