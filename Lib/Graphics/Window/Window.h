//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include "Graphics/Graph/Artists/Artist.h"
#include "Graphics/Styles/Colors.h"
#include "Graphics/Styles/WindowStyles.h"

#include "Core/Backend/Events/GUIEventListener.h"

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

    protected:
        Flags flags;
        RectI windowRect;

        void setClearColor(Styles::Color color);

    public:
        explicit Window(int x = 0, int y = 0, int w = 100, int h = 100, Flags flags = None);

        virtual void draw();

        virtual void notifyReshape(int _w, int _h);

        bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) final;

        bool notifyRender() override;

        void setDecorate(bool _decorate);

        void setClear(bool _clear);

        auto isMouseIn() const -> bool;

        auto getMouseWindowCoord() const -> Point2D;

        auto getMouseViewportCoord() const -> Point2D;

        RectI getViewport() const;

        const RectI &getWindowRect() const { return windowRect; };

        int getx() const { return windowRect.xMin; }

        int gety() const { return windowRect.yMin; }

        int getw() const { return windowRect.width(); }

        int geth() const { return windowRect.height(); }

        void setx(int x) { windowRect.xMin = x; }

        void sety(int y) { windowRect.yMin = y; }

        void setSize(int w, int h) {
            windowRect.xMax = windowRect.xMin + w;
            windowRect.yMax = windowRect.yMin + h;
        }

    };

}

#endif //V_SHAPE_WINDOW_H
