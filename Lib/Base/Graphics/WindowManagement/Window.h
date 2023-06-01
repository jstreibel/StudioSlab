//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include <vector>
#include "Base/Graphics/Artists/Artist.h"
#include "Base/Backend/GLUT/GLUTEventListener.h"

class Window : public Base::GLUTEventListener {
public:

    Window(int x=0, int y=0, int w=100, int h=100);

    virtual void draw(bool decorated=true, bool clear=true) const;

    void setupViewport(bool decorate=true, bool clear=true) const;

    virtual void reshape(int w, int h);

    int w, h, x, y;
    double winXoffset = 2;
    double winYoffset = 2;

    std::vector<Artist*> content;

    void addArtist(Artist *pArtist);

    auto hit(int x, int y) const -> bool;

    bool gotHit = false;

private:
    void _clear() const;
    void _decorate() const;

    static constexpr double p = 0.999;

};


#endif //V_SHAPE_WINDOW_H
