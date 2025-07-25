//
// Created by joao on 20/04/2021.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/SlabGraphics.h"

#include "SlabWindow.h"

#include <utility>
#include "Core/Tools/Log.h"
#include "Graphics/Utils.h"


namespace Slab::Graphics {

    #define USE_GLOBAL_MOUSECLICK_POLICY false


    FSlabWindowConfig::FSlabWindowConfig(Str Title, const RectI WinRect, const Int Flags)
    : Title(std::move(Title))
    , WinRect(WinRect)
    , Flags(Flags)
    {
    }


    FSlabWindow::FSlabWindow(FSlabWindowConfig ConfigArg)
    : Config(std::move(ConfigArg))
    , MouseState(New<FMouseState>())
    {
        if(Config.Title.empty())
        {
            Config.Title = "[Window:" + ToStr(GetId()) + "]";
        }

        AddResponder(MouseState);
    }

    FSlabWindow::~FSlabWindow() = default;

    auto FSlabWindow::GetUniqueName() const -> Str
    {
        return AddUniqueIdToString(GetTitle());
    }

    auto FSlabWindow::GetConfig() -> FSlabWindowConfig& {
        return Config;
    }

    void FSlabWindow::ImmediateDraw(const FPlatformWindow& PlatformWindow)
    {
        if (!SetupViewport(PlatformWindow)) {};
    }

    auto FSlabWindow::RegisterDeferredDrawCalls(const FPlatformWindow& PlatformWindow) -> void { }

    void FSlabWindow::NotifyReshape(int w, int h) {
        Config.WinRect.xMax = Config.WinRect.xMin + w;
        Config.WinRect.yMax = Config.WinRect.yMin + h;
    }

    bool FSlabWindow::SetupViewport(const FPlatformWindow& PlatformWindow) const {

        fix SysWinHeight = HeightOverride < 0 ? PlatformWindow.GetHeight() : HeightOverride;

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

    auto FSlabWindow::IsPointWithin(const Point2D &Point) const -> bool {
        auto GuiBackend = Slab::Graphics::GetGraphicsBackend();

        const auto Rect = GetViewport();

        fix x = Rect.xMin;
        fix y = Rect.yMin;
        fix w = Rect.GetWidth();
        fix h = Rect.GetHeight();

        return Point.x > x && Point.x < x + w && Point.y > y && Point.y < y + h;
    }

    auto FSlabWindow::FromPlatformWindowToViewportCoords(const Point2D& PointInPlatformWindowCoords) const -> Point2D {
        auto vpRect = GetViewport();

        IN Point = PointInPlatformWindowCoords;

        fix xPointLocal = Point.x - vpRect.xMin;
        // fix yMouseLocal = parent_system window_h - mouse.y - vpRect.yMin;
        fix yPointLocal = Point.y - vpRect.yMin;

        return {xPointLocal, yPointLocal};
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

    auto FSlabWindow::GetMouseState() const -> TPointer<const FMouseState>
    {
        return MouseState;
    }

    auto FSlabWindow::IsMouseInside() const -> bool
    {
        return IsPointWithin(Point2D{static_cast<double>(MouseState->x), static_cast<double>(MouseState->y)});
    }

    auto FSlabWindow::GetMouseViewportCoord() const -> Point2D
    {
        return FromPlatformWindowToViewportCoords(Point2D{MouseState->x, MouseState->y});
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

    bool FSlabWindow::NotifyRender(const FPlatformWindow& PlatformWindow) {
        ImmediateDraw(PlatformWindow);
        RegisterDeferredDrawCalls(PlatformWindow);

        return FPlatformWindowEventListener::NotifyRender(PlatformWindow);
    }

    void FSlabWindow::OverrideSystemWindowHeight(const int Height) {
        HeightOverride = Height;
    }

    auto FSlabWindow::Get_x() const -> int { return Config.WinRect.xMin; }

    auto FSlabWindow::Get_y() const -> int { return Config.WinRect.yMin; }

    auto FSlabWindow::GetWidth() const -> int { return Config.WinRect.GetWidth(); }

    auto FSlabWindow::GetHeight() const -> int { return Config.WinRect.GetHeight(); }


}
