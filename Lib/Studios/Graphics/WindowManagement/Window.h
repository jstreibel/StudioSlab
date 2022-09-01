//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


#include <vector>
#include "Studios/Graphics/Artists/Artist.h"

class Window {
public:

    Window(int x=0, int y=0, int w=100, int h=100);

    virtual void draw(bool decorated=true, bool clear=true) const;

    void setupViewport(bool decorate=true, bool clear=true) const;

    int w, h, x, y;

    std::vector<Artist*> content;

    void addArtist(Artist *pArtist);

private:
    void _clear() const;
    void _decorate() const;

    static constexpr double p = 0.999;
    double winXoffset = 2;
    double winYoffset = 2;

};


#endif //V_SHAPE_WINDOW_H
