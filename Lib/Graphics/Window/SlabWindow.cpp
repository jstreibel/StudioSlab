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


    FSlabWindow::FConfig::FConfig(FOwnerPlatformWindow  Owner, Str Title, const RectI WinRect, const Int Flags)
    : Owner(std::move(Owner))
    , Title(std::move(Title))
    , WinRect(WinRect)
    , Flags(Flags)
    {

    }


    FSlabWindow::FSlabWindow(FConfig ConfigArg)
    : FPlatformWindowEventListener(ConfigArg.Owner)
    , Config(std::move(ConfigArg))
    {
        if(Config.Title.empty())
        {
            Config.Title = "[Window:" + ToStr(GetId()) + "]";
        }
    }

    FSlabWindow::~FSlabWindow() = default;

    auto FSlabWindow::GetConfig() -> FConfig& {
        return Config;
    }

    void FSlabWindow::ImmediateDraw()
    {
        if (!SetupViewport()) {};
    }

    auto FSlabWindow::RegisterDeferredDrawCalls() -> void { }

    void FSlabWindow::NotifyReshape(int w, int h) {
        Config.WinRect.xMax = Config.WinRect.xMin + w;
        Config.WinRect.yMax = Config.WinRect.yMin + h;
    }

    bool FSlabWindow::NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) {
        if     (button == EMouseButton::MouseButton_LEFT)   MouseLeftButton   = state;
        else if(button == EMouseButton::MouseButton_MIDDLE) MouseCenterButton = state;
        else if(button == EMouseButton::MouseButton_RIGHT)  MouseRightButton  = state;

        return FPlatformWindowEventListener::NotifyMouseButton(button, state, keys);
    }

    bool FSlabWindow::NotifyMouseMotion(int x, int y, int dx, int dy) {
        return FPlatformWindowEventListener::NotifyMouseMotion(x, y, dx, dy);
    }

    bool FSlabWindow::NotifyMouseWheel(double dx, double dy) {
        return FPlatformWindowEventListener::NotifyMouseWheel(dx, dy);
    }

    bool FSlabWindow::NotifyKeyboard(Slab::Graphics::EKeyMap key, Slab::Graphics::EKeyState state,
                                    Slab::Graphics::EModKeys modKeys) {
        return FPlatformWindowEventListener::NotifyKeyboard(key, state, modKeys);
    }

    bool FSlabWindow::SetupViewport() const {
        IN ParentWindow = w_ParentPlatformWindow.lock();

        if (ParentWindow == nullptr)
        {
            Core::Log::Warning("Trying to setup Viewport on SlabWindow with no Parent Platform Window.");
            return false;
        }

        fix SysWinHeight = HeightOverride < 0 ? ParentWindow->GetHeight() : HeightOverride;

        fix Viewport = GetViewport();

        fix x = Viewport.xMin;
        fix y = SysWinHeight - Viewport.yMin - Viewport.GetHeight();

        fix Width = Viewport.GetWidth();
        fix Height = Viewport.GetHeight();

        if (Width<=0 || Height<=0)
        {
            Core::Log::Warning("Refuse to set glViewport for values w=") << Width << ", h=" << Height;
            return false;
        }

        glViewport(x, y, Width, Height);

        return true;
    }

    auto FSlabWindow::IsMouseIn() const -> bool {
        IN ParentWindow = w_ParentPlatformWindow.lock();

        if (ParentWindow == nullptr)
        {
            Core::Log::Warning("Trying get runtime mouse data in SlabWindow with no Parent Platform Window.");
            return false;
        }

        auto GuiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &Mouse = ParentWindow->GetMouseState();

        const auto Rect = GetViewport();

        fix x = Rect.xMin;
        fix y = Rect.yMin;
        fix w = Rect.GetWidth();
        fix h = Rect.GetHeight();

        return Mouse->x > x && Mouse->x < x + w && Mouse->y > y && Mouse->y < y + h;
    }

    auto FSlabWindow::IsMouseLeftClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        return mouse.leftPressed;
#else
        return MouseLeftButton==EKeyState::Press;
#endif
    }

    auto FSlabWindow::IsMouseCenterClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.centerPressed) return false;
#else
        return MouseCenterButton==EKeyState::Press;
#endif
    }

    auto FSlabWindow::IsMouseRightClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.rightPressed) return false;
#else
        return MouseRightButton==EKeyState::Press;
#endif
    }

    auto FSlabWindow::GetMouseViewportCoord() const -> Point2D {
        IN ParentWindow = w_ParentPlatformWindow.lock();

        if (ParentWindow == nullptr)
        {
            Core::Log::Warning("Trying to get runtime mouse data (viewport coord) on SlabWindow with no Parent Platform Window.");
            return {};
        }

        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = ParentWindow->GetMouseState();
        auto vpRect = GetViewport();

        fix xMouseLocal = mouse->x - vpRect.xMin;
        // fix yMouseLocal = parent_system window_h - mouse.y - vpRect.yMin;
        fix yMouseLocal = mouse->y - vpRect.yMin;

        return {static_cast<DevFloat>(xMouseLocal), static_cast<DevFloat>(yMouseLocal)};
    }



    void FSlabWindow::SetDecorate(bool decorate) {
        if(!decorate)  Config.Flags |=  SlabWindowNoDecoration;
        else           Config.Flags &= ~SlabWindowNoDecoration;
    }

    void FSlabWindow::SetClear(bool clear) {
        if(!clear) Config.Flags |=  SlabWindowDontClear;
        else       Config.Flags &= ~SlabWindowDontClear;
    }

    RectI FSlabWindow::GetViewport() const {
        return Config.WinRect;
    }

    Str FSlabWindow::GetTitle() const {
        return Config.Title;
    }

    Int FSlabWindow::GetFlags() const {
        return Config.Flags;
    }

    bool FSlabWindow::WantsFullscreen() const {
        return Config.Flags & SlabWindowWantsFullscreen;
    }

    void FSlabWindow::SetMinimumWidth(const Slab::Resolution MinWidthArg) {
        MinWidth = MinWidthArg;

        if(MinWidth > GetWidth()) NotifyReshape(static_cast<int>(MinWidth), GetHeight());
    }

    void FSlabWindow::SetMinimumHeight(const Slab::Resolution MinHeightArg) {
        MinHeight = MinHeightArg;

        if(MinHeight > GetHeight()) NotifyReshape(GetWidth(), static_cast<int>(MinHeight));
    }

    auto FSlabWindow::Set_x(const int x) -> void {
        fix w=Config.WinRect.GetWidth();
        Config.WinRect.xMin = x;
        Config.WinRect.xMax = x+w;
    }

    auto FSlabWindow::Set_y(const int y) -> void {
        fix h=Config.WinRect.GetHeight();
        Config.WinRect.yMin = y;
        Config.WinRect.yMax = y+h;
    }

    void FSlabWindow::NotifyBecameActive()   { Active = true; }

    void FSlabWindow::NotifyBecameInactive() { Active = false; }

    bool FSlabWindow::IsActive() const       { return Active; }

    bool FSlabWindow::NotifySystemWindowReshape(const int w, const int h) {
        NotifyReshape(w, h);
        // return SystemWindowEventListener::notifySystemWindowReshape(w, h);

        return false;
    }

    bool FSlabWindow::NotifyRender() {
        ImmediateDraw();
        RegisterDeferredDrawCalls();

        return FPlatformWindowEventListener::NotifyRender();
    }

    void FSlabWindow::OverrideSystemWindowHeight(const int Height) {
        HeightOverride = Height;
    }

    auto FSlabWindow::Get_x() const -> int { return Config.WinRect.xMin; }

    auto FSlabWindow::Get_y() const -> int { return Config.WinRect.yMin; }

    auto FSlabWindow::GetWidth() const -> int { return Config.WinRect.GetWidth(); }

    auto FSlabWindow::GetHeight() const -> int { return Config.WinRect.GetHeight(); }


}