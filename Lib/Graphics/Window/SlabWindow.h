//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H

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
            Str title;
            RectI win_rect;
            Int flags;
        };

    private:
        void setupWindow() const;

        KeyState mouseLeftButton = KeyState::Release;
        KeyState mouseCenterButton = KeyState::Release;
        KeyState mouseRightButton = KeyState::Release;

    protected:
        Config config;
        Resolution min_width=800, min_height=450;
        bool active=false;

    public:

        explicit SlabWindow(Config c={"", WindowStyle::default_window_rect, 0x0});

        ~SlabWindow() override;

        Int getFlags() const;

        virtual void draw();
        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;
        bool notifyMouseMotion(int x, int y, int dx, int dy) override;
        bool notifyMouseWheel(double dx, double dy) override;
        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifySystemWindowReshape(int w, int h) final;

        virtual void notifyReshape(int w, int h);
        virtual void notifyBecameActive();
        virtual void notifyBecameInactive();

        bool isActive() const;

        bool wantsFullscreen() const;

        auto isMouseIn() const -> bool;
        auto isMouseLeftClicked() const -> bool;
        auto isMouseCenterClicked() const -> bool;
        auto isMouseRightClicked() const -> bool;
        auto getMouseViewportCoord() const -> Point2D;

        Str getTitle() const;

        void setDecorate(bool);
        void setClear(bool);

        RectI getViewport() const;

        inline auto getx() const -> int  { return config.win_rect.xMin; }
        inline auto gety() const -> int  { return config.win_rect.yMin; }
        inline auto GetWidth() const -> int  { return config.win_rect.width(); }
        inline auto GetHeight() const -> int  { return config.win_rect.height(); }
        void SetMinimumWidth(Resolution);
        void SetMinimumHeight(Resolution);


        virtual auto setx(int x)  -> void;
        virtual auto sety(int y)  -> void;

    };

    DefinePointers(SlabWindow)

}

#endif //V_SHAPE_WINDOW_H
