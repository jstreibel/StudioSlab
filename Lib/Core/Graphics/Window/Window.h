//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include "Core/Graphics/Artists/Artist.h"
#include "Core/Backend/Events/EventListener.h"
#include "Core/Graphics/Artists/Rect.h"

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
    int w, h, x, y;

    bool gotHit = false;

public:
    typedef std::shared_ptr<Window> Ptr;

    Window(int x=0, int y=0, int w=100, int h=100);

    void addArtist(Artist *pArtist);

    virtual void draw();
    virtual void notifyReshape(int newWinW, int newWinH);
    virtual IntPair getWindowSizeHint();

    bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) final;

    bool notifyRender(float elTime_msec) override;

    void setDecorate(bool _decorate);
    void setClear(bool _clear);

    auto doesHit(int xMouse, int yMouse) const -> bool;

    RectI getViewport() const;
    int getx() const { return x; }
    int gety() const { return y; }
    int getw() const { return w; }
    int geth() const { return h; }
    void setx(int _x) { this->x = _x; }
    void sety(int _y) { this->y = _y; }
    void setSize(int _w, int _h) { this->notifyReshape(_w, _h); }

};


#endif //V_SHAPE_WINDOW_H
