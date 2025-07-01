//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H

// #include <crude_json.h>

#include "Graphics/Styles/Colors.h"

#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Backend/Events/KeyMap.h"
#include "Graphics/Types2D.h"
#include "Graphics/Window/WindowStyles.h"
#include "Core/Tools/UniqueObject.h"
#include "Graphics/Backend/PlatformWindow.h"

#include <vector>
#include <memory>


namespace Slab::Graphics {

    enum SlabWindowFlags {
        // HasMainMenu  = 0x1,
        SlabWindowNoDecoration = 0x2,
        SlabWindowDontClear    = 0x4,
        SlabWindowWantsFullscreen   = 0x8
    };

    struct FSlabWindowConfig {
        explicit FSlabWindowConfig(
            Str Title="",
            RectI WinRect=WindowStyle::DefaultWindowRect,
            Int Flags=0x0);

        Str Title;
        RectI WinRect = WindowStyle::DefaultWindowRect;
        Int Flags = 0x0;
    };

    class FSlabWindow : protected Core::UniqueObject, public FPlatformWindowEventListener {
        friend class SlabWindowManager;

        [[nodiscard]] bool SetupViewport(const FPlatformWindow& PlatformWindow) const;

        Pointer<FMouseState> MouseState;

    protected:
        FSlabWindowConfig Config;
        Resolution MinWidth=800, MinHeight=450;
        Int HeightOverride = -1;
        bool Active=false;

    public:

        explicit FSlabWindow(FSlabWindowConfig ConfigArg);

        ~FSlabWindow() override;

        auto GetConfig() -> FSlabWindowConfig&;

        auto NotifySystemWindowReshape(int w, int h)           -> bool final;
        auto NotifyRender(const FPlatformWindow&)              -> bool final;

        /**
         *  Override the value used to compute viewport positions in OpenGL environments.
         *
         * @param Height the value to override the system window height. Set to negative to stop override.
         */
        void OverrideSystemWindowHeight(int Height);

        /**
         * This function is called when the window should render it's content immediately to its output.
         */
        virtual auto ImmediateDraw(const FPlatformWindow&)     -> void;
        /**
         * Here the window has the opportunity to register callbacks and other things (e.g. ImGui window list
         * draw callback commands).
         */
        virtual auto RegisterDeferredDrawCalls(const FPlatformWindow&)               -> void;

        virtual auto NotifyReshape(int w, int h)               -> void;
        virtual auto NotifyBecameActive()                      -> void;
        virtual auto NotifyBecameInactive()                    -> void;
        virtual auto Set_x(int x)                              -> void;
        virtual auto Set_y(int y)                              -> void;

        [[nodiscard]] auto GetFlags()                    const -> Int;
        [[nodiscard]] auto IsActive()                    const -> bool;
        [[nodiscard]] auto WantsFullscreen()             const -> bool;

        [[nodiscard]] auto GetMouseState()               const -> Pointer<const FMouseState>;
        [[nodiscard]] auto IsMouseInside()               const -> bool;
        [[nodiscard]] auto GetMouseViewportCoord()       const -> Point2D;
        [[nodiscard]] virtual
        auto IsPointWithin(const Point2D&)               const -> bool;
        [[nodiscard]] virtual
        auto FromPlatformWindowToViewportCoords(const Point2D&) const -> Point2D;

        [[nodiscard]] auto GetTitle()                    const -> Str;

        auto SetDecorate(bool)                                 -> void;
        auto SetClear(bool)                                    -> void;

        auto GetViewport()                               const -> RectI;
        auto Get_x()                                     const -> int;
        auto Get_y()                                     const -> int;
        auto GetWidth()                                  const -> int;
        auto GetHeight()                                 const -> int;
        auto SetMinimumWidth(Resolution)                       -> void;
        auto SetMinimumHeight(Resolution)                      -> void;
    };

    DefinePointers(FSlabWindow)

}

#endif //V_SHAPE_WINDOW_H
