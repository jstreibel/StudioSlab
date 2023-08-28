//
// Created by joao on 3/30/23.
//

#ifndef STUDIOSLAB_RECT_H
#define STUDIOSLAB_RECT_H

#include <Common/Types.h>

template<typename T>
struct Rect {
    T xMin, xMax, yMin, yMax;

    T w() const {return xMax-xMin; }
    T h() const {return yMax-yMin; }
};

typedef Rect<int> RectI;
typedef Rect<Real> RectR;

#endif //STUDIOSLAB_RECT_H
