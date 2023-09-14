//
// Created by joao on 3/30/23.
//

#ifndef STUDIOSLAB_RECT_H
#define STUDIOSLAB_RECT_H

#include <Utils/Types.h>

template<typename T>
struct Rect {
    T xMin, xMax, yMin, yMax;

    T width() const {return xMax-xMin; }
    T height() const {return yMax - yMin; }

    T yCenter() const { return .5 * (yMax + yMin); }
    T xCenter() const { return .5 * (xMax + xMin); }

    bool doesHit(const T &x, const T &y) const { return x>=xMin && x<=xMax && y>=yMin && y<=yMax; }
};

typedef Rect<int> RectI;
typedef Rect<Real> RectR;

#endif //STUDIOSLAB_RECT_H
