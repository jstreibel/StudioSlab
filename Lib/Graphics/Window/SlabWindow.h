//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H

#include <crude_json.h>

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

    class SlabWindow : protected Core::UniqueObject, public SystemWindowEventListener {
        friend class SlabWindowManager;

    public:

        struct Config {
            Config(const Str &title="",
                RectI win_rect=WindowStyle::default_window_rect,
                Int flags=0x0,
                ParentSystemWindow parent_syswin=nullptr);

            Str title;
            RectI win_rect;
            Int flags;
            ParentSystemWindow parent_syswin;
        };

    private:
        void setupWindow() const;

        KeyState mouseLeftButton   = KeyState::Release;
        KeyState mouseCenterButton = KeyState::Release;
        KeyState mouseRightButton  = KeyState::Release;

    protected:
        Config config;
        Resolution min_width=800, min_height=450;
        Int h_override = -1;
        bool active=false;

    public:

        explicit SlabWindow(Config c=Config());

        ~SlabWindow() override;

        auto getConfig() -> Config&;

        auto notifySystemWindowReshape(int w, int h)           -> bool final;
        auto notifyRender()                                    -> bool final;

        /**
         *  Override the value used to compute viewport positions in OpenGL environments.
         *
         * @param override_h the value to override the system window height. Set to negative to stop override.
         */
        void overrideSystemWindowHeight(int override_h);

        auto notifyMouseButton(MouseButton, KeyState, ModKeys) -> bool override;
        auto notifyMouseMotion(int x, int y, int dx, int dy)   -> bool override;
        auto notifyMouseWheel(double dx, double dy)            -> bool override;
        auto notifyKeyboard(KeyMap, KeyState, ModKeys)         -> bool override;

        virtual auto draw()                                    -> void;
        virtual auto notifyReshape(int w, int h)               -> void;
        virtual auto notifyBecameActive()                      -> void;
        virtual auto notifyBecameInactive()                    -> void;
        virtual auto setx(int x)                               -> void;
        virtual auto sety(int y)                               -> void;

        auto getFlags()                                  const -> Int;
        auto isActive()                                  const -> bool;
        auto wantsFullscreen()                           const -> bool;
        auto isMouseIn()                                 const -> bool;
        auto isMouseLeftClicked()                        const -> bool;
        auto isMouseCenterClicked()                      const -> bool;
        auto isMouseRightClicked()                       const -> bool;
        auto getMouseViewportCoord()                     const -> Point2D;

        auto getTitle()                                  const -> Str;

        auto setDecorate(bool)                                 -> void;
        auto setClear(bool)                                    -> void;

        RectI getViewport() const;

        auto getx()                                      const -> int;
        auto gety()                                      const -> int;
        auto GetWidth()                                  const -> int;
        auto GetHeight()                                 const -> int;
        auto SetMinimumWidth(Resolution)                       -> void;
        auto SetMinimumHeight(Resolution)                      -> void;
    };

    DefinePointers(SlabWindow)

}

#endif //V_SHAPE_WINDOW_H
