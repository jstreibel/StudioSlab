//
// Created by joao on 24/09/2019.
//

#include "Categories.h"

OStream& operator<< (OStream& stream, const Real2D& x) {
    return stream << "(" << x.x << "," << x.y << ")";
}