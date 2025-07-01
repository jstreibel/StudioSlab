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

    class FSlabWindow : protected Core::UniqueObject, public FPlatformWindowEventListener {
        friend class SlabWindowManager;

    public:

        struct FConfig {
            FConfig() = delete;
            explicit FConfig(
                Str Title="",
                RectI WinRect=WindowStyle::DefaultWindowRect,
                Int Flags=0x0);

            Str Title = "";
            RectI WinRect = WindowStyle::DefaultWindowRect;
            Int Flags = 0x0;

            /**
             * This is the same Owner as the super class FPlatformWindowEventListener::w_ParentPlatformWindow.
             * While FPlatformWindowEventListener itself does not use its parent platform window, FSlabWindow
             * does use it. So, if a subclass does not need w_ParentPlatformWindow, it can hide its
             * implicit usage by private'ing the following methods:
             *
             * SetupViewport()
             * IsMouseIn()
             * GetMouseViewportCoord()
             */
            FOwnerPlatformWindow Owner;
        };

    private:
        /**
         * Sets a viewport region in the current platform window for drawing.
         * Makes use of w_ParentWindowPlatform.
         * @return False if no w_ParentWindowPlatform is set. True otherwise.
         */
        [[nodiscard]] virtual bool SetupViewport() const;

        EKeyState MouseLeftButton   = EKeyState::Release;
        EKeyState MouseCenterButton = EKeyState::Release;
        EKeyState MouseRightButton  = EKeyState::Release;

    protected:
        FConfig Config;
        Resolution MinWidth=800, MinHeight=450;
        Int HeightOverride = -1;
        bool Active=false;

    public:

        explicit FSlabWindow(FConfig ConfigArg);

        ~FSlabWindow() override;

        auto GetConfig() -> FConfig&;

        auto NotifySystemWindowReshape(int w, int h)           -> bool final;
        auto NotifyRender()                                    -> bool final;

        /**
         *  Override the value used to compute viewport positions in OpenGL environments.
         *
         * @param Height the value to override the system window height. Set to negative to stop override.
         */
        void OverrideSystemWindowHeight(int Height);

        auto NotifyMouseButton(EMouseButton, EKeyState, EModKeys) -> bool override;
        auto NotifyMouseMotion(int x, int y, int dx, int dy)   -> bool override;
        auto NotifyMouseWheel(double dx, double dy)            -> bool override;
        auto NotifyKeyboard(EKeyMap, EKeyState, EModKeys)         -> bool override;

        /**
         * This function is called when the window should render it's content immediately to its output.
         */
        virtual auto ImmediateDraw()                           -> void;
        /**
         * Here the window has the opportunity to register callbacks and other things (e.g. ImGui window list
         * draw callback commands).
         */
        virtual auto RegisterDeferredDrawCalls()               -> void;

        virtual auto NotifyReshape(int w, int h)               -> void;
        virtual auto NotifyBecameActive()                      -> void;
        virtual auto NotifyBecameInactive()                    -> void;
        virtual auto Set_x(int x)                              -> void;
        virtual auto Set_y(int y)                              -> void;

        auto GetFlags()                                  const -> Int;
        auto IsActive()                                  const -> bool;
        auto WantsFullscreen()                           const -> bool;
        virtual auto IsMouseIn()                         const -> bool;
        auto IsMouseLeftClicked()                        const -> bool;
        auto IsMouseCenterClicked()                      const -> bool;
        auto IsMouseRightClicked()                       const -> bool;
        virtual auto GetMouseViewportCoord()             const -> Point2D;

        auto GetTitle()                                  const -> Str;

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
