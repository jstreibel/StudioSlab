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


    FSlabWindow::Config::Config(const Str &title, RectI win_rect, Int flags, FOwnerSystemWindow Owner)
    : title(title), win_rect(win_rect), flags(flags), Owner(Owner) {
        if (Owner == nullptr) {
            this->Owner = &*GetGraphicsBackend()->GetMainSystemWindow();
        }
    }


    FSlabWindow::
    FSlabWindow(Config cfg)
    : FSystemWindowEventListener(cfg.Owner), config(std::move(cfg))
    {
        if(config.title.empty()){
            config.title = "[Window:" + ToStr(GetId()) + "]";
        }

        if(parent_system_window == nullptr)
            parent_system_window = &*GetGraphicsBackend()->GetMainSystemWindow();
    }

    FSlabWindow::~FSlabWindow() = default;

    auto FSlabWindow::getConfig() -> Config& {
        return config;
    }

    void FSlabWindow::Draw() {
        SetupWindow();
    }

    void FSlabWindow::NotifyReshape(int w, int h) {
        config.win_rect.xMax = config.win_rect.xMin + w;
        config.win_rect.yMax = config.win_rect.yMin + h;
    }

    bool FSlabWindow::NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if     (button == MouseButton::MouseButton_LEFT)   mouseLeftButton   = state;
        else if(button == MouseButton::MouseButton_MIDDLE) mouseCenterButton = state;
        else if(button == MouseButton::MouseButton_RIGHT)  mouseRightButton  = state;

        return FSystemWindowEventListener::NotifyMouseButton(button, state, keys);
    }

    bool FSlabWindow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        return FSystemWindowEventListener::NotifyMouseMotion(x, y, dx, dy);
    }

    bool FSlabWindow::NotifyMouseWheel(double dx, double dy) {
        return FSystemWindowEventListener::NotifyMouseWheel(dx, dy);
    }

    bool FSlabWindow::NotifyKeyboard(Slab::Graphics::KeyMap key, Slab::Graphics::KeyState state,
                                    Slab::Graphics::ModKeys modKeys) {
        return FSystemWindowEventListener::NotifyKeyboard(key, state, modKeys);
    }

    void FSlabWindow::SetupWindow() const {
        fix syswin_h = h_override < 0 ? parent_system_window->GetHeight() : h_override;

        fix vp = getViewport();

        fix x = vp.xMin;
        fix y = syswin_h - vp.yMin - vp.height();

        fix w = vp.width();
        fix h = vp.height();

        if (w<=0 || h<=0)
        {
            Core::Log::Warning("Refuse to set glViewport for values w=") << w << ", h=" << h;
            return;
        }

        glViewport(x, y, w, h);
    }

    auto FSlabWindow::isMouseIn() const -> bool {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = parent_system_window->GetMouseState();

        auto rect = getViewport();

        fix x = rect.xMin;
        fix y = rect.yMin;
        fix w = rect.width();
        fix h = rect.height();

        return mouse->x > x && mouse->x < x + w && mouse->y > y && mouse->y < y + h;
    }

    auto FSlabWindow::isMouseLeftClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        return mouse.leftPressed;
#else
        return mouseLeftButton==KeyState::Press;
#endif
    }

    auto FSlabWindow::isMouseCenterClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.centerPressed) return false;
#else
        return mouseCenterButton==KeyState::Press;
#endif
    }

    auto FSlabWindow::isMouseRightClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.rightPressed) return false;
#else
        return mouseRightButton==KeyState::Press;
#endif
    }

    auto FSlabWindow::getMouseViewportCoord() const -> Point2D {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = parent_system_window->GetMouseState();
        auto vpRect = getViewport();

        fix xMouseLocal = mouse->x - vpRect.xMin;
        // fix yMouseLocal = parent_systemwindow_h - mouse.y - vpRect.yMin;
        fix yMouseLocal = mouse->y - vpRect.yMin;

        return {(DevFloat) xMouseLocal, (DevFloat) yMouseLocal};
    }



    void FSlabWindow::SetDecorate(bool decorate) {
        if(!decorate)  config.flags |=  SlabWindowNoDecoration;
        else           config.flags &= ~SlabWindowNoDecoration;
    }

    void FSlabWindow::SetClear(bool clear) {
        if(!clear) config.flags |=  SlabWindowDontClear;
        else       config.flags &= ~SlabWindowDontClear;
    }

    RectI FSlabWindow::getViewport() const {
        return config.win_rect;
    }

    Str FSlabWindow::getTitle() const {
        return config.title;
    }

    Int FSlabWindow::getFlags() const {
        return config.flags;
    }

    bool FSlabWindow::wantsFullscreen() const {
        return config.flags & SlabWindowWantsFullscreen;
    }

    void FSlabWindow::SetMinimumWidth(Slab::Resolution minw) {
        min_width = minw;

        if(min_width > GetWidth()) NotifyReshape((int)min_width, GetHeight());
    }

    void FSlabWindow::SetMinimumHeight(Slab::Resolution minh) {
        min_height = minh;

        if(min_height > GetHeight()) NotifyReshape(GetWidth(), (int)min_height);
    }

    auto FSlabWindow::Set_x(const int x) -> void {
        fix w=config.win_rect.width();
        config.win_rect.xMin = x;
        config.win_rect.xMax = x+w;
    }

    auto FSlabWindow::Set_y(const int y) -> void {
        fix h=config.win_rect.height();
        config.win_rect.yMin = y;
        config.win_rect.yMax = y+h;
    }

    void FSlabWindow::notifyBecameActive()   { active = true; }

    void FSlabWindow::notifyBecameInactive() { active = false; }

    bool FSlabWindow::isActive() const       { return active; }

    bool FSlabWindow::NotifySystemWindowReshape(const int w, const int h) {
        NotifyReshape(w, h);
        // return SystemWindowEventListener::notifySystemWindowReshape(w, h);

        return false;
    }

    bool FSlabWindow::NotifyRender() {
        Draw();

        return FSystemWindowEventListener::NotifyRender();
    }

    void FSlabWindow::overrideSystemWindowHeight(const int override_h) {
        h_override = override_h;
    }

    auto FSlabWindow::getx() const -> int { return config.win_rect.xMin; }

    auto FSlabWindow::gety() const -> int { return config.win_rect.yMin; }

    auto FSlabWindow::GetWidth() const -> int { return config.win_rect.width(); }

    auto FSlabWindow::GetHeight() const -> int { return config.win_rect.height(); }


}