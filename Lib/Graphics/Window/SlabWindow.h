//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H

#include "Graphics/Styles/Colors.h"

#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Backend/Events/KeyMap.h"
#include "Graphics/Types2D.h"

#include <vector>
#include <memory>


namespace Slab::Graphics {

    class SlabWindow {
        friend class SlabWindowManager;
        int parent_systemwindow_h;

    public:
        virtual void setupParentSystemWindowHeight(Int);

    public:
        enum Flags {
            HasMainMenu  = 0x1,
            NoDecoration = 0x2,
            DontClear    = 0x4,
            WantsFullscreen   = 0x8
        };

    private:
        void setupWindow() const;

        KeyState mouseLeftButton = KeyState::Release;
        KeyState mouseCenterButton = KeyState::Release;
        KeyState mouseRightButton = KeyState::Release;

    protected:
        Int flags;
        RectI windowRect;

    public:
        explicit SlabWindow(int x = 100, int y = 100, int w = 800, int h = 480, Int flags=0x0);
        virtual ~SlabWindow();

        Int getFlags() const;

        virtual void draw();
        virtual bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys);
        virtual bool notifyMouseMotion(int x, int y);
        virtual bool notifyMouseWheel(double dx, double dy);
        virtual bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys);
        virtual void notifyReshape(int w, int h);

        bool wantsFullscreen() const;

        auto isMouseIn() const -> bool;
        auto isMouseLeftClicked() const -> bool;
        auto isMouseCenterClicked() const -> bool;
        auto isMouseRightClicked() const -> bool;
        auto getMouseViewportCoord() const -> Point2D;

        bool isActive() const;

        void setDecorate(bool);
        void setClear(bool);

        RectI getViewport() const;

        bool hasMainMenu() const;

        inline auto getx() const -> int  { return windowRect.xMin; }
        inline auto gety() const -> int  { return windowRect.yMin; }
        inline auto getw() const -> int  { return windowRect.width(); }
        inline auto geth() const -> int  { return windowRect.height(); }
        inline auto setx(int x)  -> void { windowRect.xMin = x; }
        inline auto sety(int y)  -> void { windowRect.yMin = y; }
        inline void setSize(int w, int h) {
            windowRect.xMax = windowRect.xMin + w;
            windowRect.yMax = windowRect.yMin + h;
        }

    };

    DefinePointers(SlabWindow)

}

#endif //V_SHAPE_WINDOW_H
