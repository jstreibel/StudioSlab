//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include <vector>
#include "Base/Graphics/Artists/Artist.h"
#include "Base/Backend/GLUT/GLUTEventListener.h"

class Window : public Base::GLUTEventListener {
    std::vector<Artist*> content;

public:

    Window(int x=0, int y=0, int w=100, int h=100);

    void addArtist(Artist *pArtist);

    virtual void draw();
    virtual void notifyReshape(int newWinW, int newWinH);
    virtual IntPair getWindowSizeHint();

    void notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;

    void setDecorate(bool _decorate);
    void setClear(bool _clear);

    auto doesHit(int x, int y) const -> bool;

    int getx() const { return x; }
    int gety() const { return y; }
    int getw() const { return w; }
    int geth() const { return h; }
    void setx(int x) { this->x = x; }
    void sety(int y) { this->y = y; }
    void setw(int w) { this->w = w; }
    void seth(int h) { this->h = h; }

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
