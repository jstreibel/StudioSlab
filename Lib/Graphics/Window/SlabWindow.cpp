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


    SlabWindow::Config::Config(const Str &title, RectI win_rect, Int flags, ParentSystemWindow parent_syswin)
    : title(title), win_rect(win_rect), flags(flags), parent_syswin(parent_syswin) {
        if (parent_syswin == nullptr) {
            this->parent_syswin = &*GetGraphicsBackend()->GetMainSystemWindow();
        }
    }


    SlabWindow::
    SlabWindow(Config cfg)
    : SystemWindowEventListener(cfg.parent_syswin), config(std::move(cfg))
    {
        if(config.title.empty()){
            config.title = "[Window:" + ToStr(GetId()) + "]";
        }

        if(parent_system_window == nullptr)
            parent_system_window = &*GetGraphicsBackend()->GetMainSystemWindow();
    }

    SlabWindow::~SlabWindow() = default;

    auto SlabWindow::getConfig() -> Config& {
        return config;
    }

    void SlabWindow::draw() {
        setupWindow();
    }

    void SlabWindow::notifyReshape(int w, int h) {
        config.win_rect.xMax = config.win_rect.xMin + w;
        config.win_rect.yMax = config.win_rect.yMin + h;
    }

    bool SlabWindow::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if     (button == MouseButton::MouseButton_LEFT)   mouseLeftButton   = state;
        else if(button == MouseButton::MouseButton_MIDDLE) mouseCenterButton = state;
        else if(button == MouseButton::MouseButton_RIGHT)  mouseRightButton  = state;

        return SystemWindowEventListener::notifyMouseButton(button, state, keys);
    }

    bool SlabWindow::notifyMouseMotion(int x, int y, int dx, int dy) {
        return SystemWindowEventListener::notifyMouseMotion(x, y, dx, dy);
    }

    bool SlabWindow::notifyMouseWheel(double dx, double dy) {
        return SystemWindowEventListener::notifyMouseWheel(dx, dy);
    }

    bool SlabWindow::notifyKeyboard(Slab::Graphics::KeyMap key, Slab::Graphics::KeyState state,
                                    Slab::Graphics::ModKeys modKeys) {
        return SystemWindowEventListener::notifyKeyboard(key, state, modKeys);
    }

    void SlabWindow::setupWindow() const {
        fix syswin_h = h_override < 0 ? parent_system_window->getHeight() : h_override;

        fix vp = getViewport();

        fix x = vp.xMin;
        fix y = syswin_h - vp.yMin - vp.height();

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

        return {(DevFloat) xMouseLocal, (DevFloat) yMouseLocal};
    }



    void SlabWindow::setDecorate(bool decorate) {
        if(!decorate)  config.flags |=  SlabWindowNoDecoration;
        else           config.flags &= ~SlabWindowNoDecoration;
    }

    void SlabWindow::setClear(bool clear) {
        if(!clear) config.flags |=  SlabWindowDontClear;
        else       config.flags &= ~SlabWindowDontClear;
    }

    RectI SlabWindow::getViewport() const {
        return config.win_rect;
    }

    Str SlabWindow::getTitle() const {
        return config.title;
    }

    Int SlabWindow::getFlags() const {
        return config.flags;
    }

    bool SlabWindow::wantsFullscreen() const {
        return config.flags & SlabWindowWantsFullscreen;
    }

    void SlabWindow::SetMinimumWidth(Slab::Resolution minw) {
        min_width = minw;

        if(min_width > GetWidth()) notifyReshape((int)min_width, GetHeight());
    }

    void SlabWindow::SetMinimumHeight(Slab::Resolution minh) {
        min_height = minh;

        if(min_height > GetHeight()) notifyReshape(GetWidth(), (int)min_height);
    }

    auto SlabWindow::setx(const int x) -> void {
        fix w=config.win_rect.width();
        config.win_rect.xMin = x;
        config.win_rect.xMax = x+w;
    }

    auto SlabWindow::sety(const int y) -> void {
        fix h=config.win_rect.height();
        config.win_rect.yMin = y;
        config.win_rect.yMax = y+h;
    }

    void SlabWindow::notifyBecameActive()   { active = true; }

    void SlabWindow::notifyBecameInactive() { active = false; }

    bool SlabWindow::isActive() const       { return active; }

    bool SlabWindow::notifySystemWindowReshape(const int w, const int h) {
        notifyReshape(w, h);
        // return SystemWindowEventListener::notifySystemWindowReshape(w, h);

        return false;
    }

    bool SlabWindow::notifyRender() {
        draw();

        return SystemWindowEventListener::notifyRender();
    }

    void SlabWindow::overrideSystemWindowHeight(const int override_h) {
        h_override = override_h;
    }

    auto SlabWindow::getx() const -> int { return config.win_rect.xMin; }

    auto SlabWindow::gety() const -> int { return config.win_rect.yMin; }

    auto SlabWindow::GetWidth() const -> int { return config.win_rect.width(); }

    auto SlabWindow::GetHeight() const -> int { return config.win_rect.height(); }


}