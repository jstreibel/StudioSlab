//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_POINT_H
#define STUDIOSLAB_POINT_H


#include "Common/CoreNativeTypes.h"

namespace Spaces {

    struct Point2D {
        Point2D() = default;

        Real x, y;
    };

    typedef std::vector<Point2D> Point2DVec;
}


#endif //STUDIOSLAB_POINT_H
