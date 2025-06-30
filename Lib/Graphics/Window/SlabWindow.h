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
#include "Graphics/Backend/SystemWindow.h"

#include <vector>
#include <memory>


namespace Slab::Graphics {

    enum SlabWindowFlags {
        // HasMainMenu  = 0x1,
        SlabWindowNoDecoration = 0x2,
        SlabWindowDontClear    = 0x4,
        SlabWindowWantsFullscreen   = 0x8
    };

    class FSlabWindow : protected Core::UniqueObject, public FSystemWindowEventListener {
        friend class SlabWindowManager;

    public:

        struct Config {
            explicit Config(const Str &title="",
                            RectI win_rect=WindowStyle::default_window_rect,
                            Int flags=0x0,
                            FOwnerSystemWindow Owner=nullptr);

            Str title;
            RectI win_rect;
            Int flags;
            FOwnerSystemWindow Owner;
        };

    private:
        void SetupWindow() const;

        EKeyState mouseLeftButton   = EKeyState::Release;
        EKeyState mouseCenterButton = EKeyState::Release;
        EKeyState mouseRightButton  = EKeyState::Release;

    protected:
        Config config;
        Resolution min_width=800, min_height=450;
        Int h_override = -1;
        bool active=false;

    public:

        explicit FSlabWindow(Config c=Config());

        ~FSlabWindow() override;

        auto getConfig() -> Config&;

        auto NotifySystemWindowReshape(int w, int h)           -> bool final;
        auto NotifyRender()                                    -> bool final;

        /**
         *  Override the value used to compute viewport positions in OpenGL environments.
         *
         * @param override_h the value to override the system window height. Set to negative to stop override.
         */
        void overrideSystemWindowHeight(int override_h);

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
        virtual auto RegisterDeferredDrawCalls()                 -> void;
        virtual auto NotifyReshape(int w, int h)               -> void;
        virtual auto notifyBecameActive()                      -> void;
        virtual auto notifyBecameInactive()                    -> void;
        virtual auto Set_x(int x)                              -> void;
        virtual auto Set_y(int y)                              -> void;

        auto getFlags()                                  const -> Int;
        auto isActive()                                  const -> bool;
        auto wantsFullscreen()                           const -> bool;
        auto IsMouseIn()                                 const -> bool;
        auto IsMouseLeftClicked()                        const -> bool;
        auto IsMouseCenterClicked()                      const -> bool;
        auto IsMouseRightClicked()                       const -> bool;
        auto GetMouseViewportCoord()                     const -> Point2D;

        auto GetTitle()                                  const -> Str;

        auto SetDecorate(bool)                                 -> void;
        auto SetClear(bool)                                    -> void;

        RectI GetViewport() const;

        auto Get_x()                                      const -> int;
        auto Get_y()                                      const -> int;
        auto GetWidth()                                  const -> int;
        auto GetHeight()                                 const -> int;
        auto SetMinimumWidth(Resolution)                       -> void;
        auto SetMinimumHeight(Resolution)                      -> void;
    };

    DefinePointers(FSlabWindow)

}

#endif //V_SHAPE_WINDOW_H
