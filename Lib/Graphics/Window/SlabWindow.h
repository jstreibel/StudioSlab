//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H

#include "Graphics/Styles/Colors.h"
#include "WindowStyles.h"

#include "Graphics/SlabGraphics.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Types2D.h"

#include <vector>
#include <memory>


namespace Slab::Graphics {

    class SlabWindow {
        friend class SlabWindowManager;

    public:
        enum Flags {
            None = 0x0,
            HasMainMenu = 0x1
        };

    private:
        bool decorate = true;
        bool clear = true;
        Color backgroundColor;

        static constexpr Real p = 0.999;

        void _clear() const;

        void _decorate() const;

        void setupWindow() const;

        KeyState mouseLeftButton = KeyState::Release;
        KeyState mouseCenterButton = KeyState::Release;
        KeyState mouseRightButton = KeyState::Release;

    protected:
        Flags flags;
        RectI windowRect;

        void setBGColor(Color color);

    public:
        explicit SlabWindow(int x = 100, int y = 100, int w = 800, int h = 480, Flags flags = None);
        virtual ~SlabWindow();

        virtual void draw();
        virtual bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys);
        virtual bool notifyMouseMotion(int x, int y);
        virtual bool notifyMouseWheel(double dx, double dy);
        virtual bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys);
        virtual void notifyReshape(int w, int h);
        virtual bool isFullscreen() const;

        auto isMouseIn() const -> bool;
        auto isMouseLeftClicked() const -> bool;
        auto isMouseCenterClicked() const -> bool;
        auto isMouseRightClicked() const -> bool;
        auto getMouseWindowCoord() const -> Point2D;
        auto getMouseViewportCoord() const -> Point2D;

        void setDecorate(bool);
        void setClear(bool);
        auto getBGColor() const -> const Color&;

        RectI getEffectiveViewport() const;

        const RectI &getTotalWindowRect() const { return windowRect; };

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
