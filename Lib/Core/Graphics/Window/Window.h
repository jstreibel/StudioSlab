//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include "Core/Graphics/Artists/Artist.h"
#include "Core/Backend/Events/EventListener.h"
#include "Core/Graphics/Artists/Rect.h"
#include "Core/Graphics/Styles/Colors.h"
#include "Core/Graphics/Styles/WindowStyles.h"

#include <vector>
#include <memory>

class Window : public Core::EventListener {
    std::vector<Artist*> content;

    bool decorate = true;
    bool clear = true;

    static constexpr Real p = 0.999;

    void _clear() const;
    void _decorate() const;
    void setupWindow() const;

protected:
    RectI windowRect;

    Styles::Color backgroundColor = Core::Graphics::backgroundColor;


public:
    typedef std::shared_ptr<Window> Ptr;

    explicit Window(int x=0, int y=0, int w=100, int h=100);

    void addArtist(Artist *pArtist);

    virtual void draw();
    virtual void notifyReshape(int _w, int _h);
    virtual IntPair getWindowSizeHint();

    bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) final;
    bool notifyRender(float elTime_msec) override;

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
        windowRect.xMax = windowRect.xMin+w;
        windowRect.yMax = windowRect.yMin+h;
    }

};


#endif //V_SHAPE_WINDOW_H
