//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include "Base/Graphics/Artists/Artist.h"
#include "Base/Backend/Events/EventListener.h"

#include <vector>
#include <memory>

class Window : public Base::EventListener {
    std::vector<Artist*> content;

public:
    typedef std::shared_ptr<Window> Ptr;

    Window(int x=0, int y=0, int w=100, int h=100);

    void addArtist(Artist *pArtist);

    virtual void draw();
    virtual void notifyReshape(int newWinW, int newWinH);
    virtual IntPair getWindowSizeHint();

    bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;

    void setDecorate(bool _decorate);
    void setClear(bool _clear);

    auto doesHit(int xMouse, int yMouse) const -> bool;

    int getx() const { return x; }
    int gety() const { return y; }
    int getw() const { return w; }
    int geth() const { return h; }
    void setx(int _x) { this->x = _x; }
    void sety(int _y) { this->y = _y; }
    void setSize(int _w, int _h) { this->notifyReshape(_w, _h); }

protected:
    int w, h, x, y;

    double winXoffset = 2;
    double winYoffset = 2;

    bool gotHit = false;

private:

    bool decorate = true;
    bool clear = true;

    void _clear() const;
    void _decorate() const;
    void _setupViewport() const;

    static constexpr double p = 0.999;

};


#endif //V_SHAPE_WINDOW_H
