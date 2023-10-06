//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include "Graphics/Graph/Artists/Artist.h"
#include "Graphics/Styles/Colors.h"
#include "Graphics/Styles/WindowStyles.h"

#include "Core/Backend/Events/GUIEventListener.h"
#include "Core/Backend/Events/MouseState.h"

#include <vector>
#include <memory>


namespace Graphics {

    class Window : public Core::GUIEventListener {
    public:
        typedef std::shared_ptr<Window> Ptr;
        enum Flags {
            None = 0x0,
            HasMainMenu = 0x1
        };

    private:
        bool decorate = true;
        bool clear = true;
        Styles::Color clearColor;

        static constexpr Real p = 0.999;

        void _clear() const;

        void _decorate() const;

        void setupWindow() const;

        Core::KeyState mouseLeftButton = Core::Release;
        Core::KeyState mouseCenterButton = Core::Release;
        Core::KeyState mouseRightButton = Core::Release;

    protected:
        Flags flags;
        RectI windowRect;

        void setClearColor(Styles::Color color);

    public:
        explicit Window(int x = 0, int y = 0, int w = 100, int h = 100, Flags flags = None);

        virtual void draw();

        virtual void notifyReshape(int w, int h);

        bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) final;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state,
                               Core::ModKeys keys) override;

        bool notifyRender() override;

        void setDecorate(bool _decorate);

        void setClear(bool _clear);

        auto isMouseIn() const -> bool;
        auto isMouseLeftClicked() const -> bool;
        auto isMouseCenterClicked() const -> bool;
        auto isMouseRightClicked() const -> bool;
        auto getMouseWindowCoord() const -> Point2D;
        auto getMouseViewportCoord() const -> Point2D;

        RectI getViewport() const;

        const RectI &getWindowRect() const { return windowRect; };

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

}

#endif //V_SHAPE_WINDOW_H
