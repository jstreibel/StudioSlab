//
// Created by joao on 24/09/2019.
//

#include "Primitives.h"

std::ostream& operator<< (std::ostream& stream, const Real2D& x) {
    return stream << "(" << x.x << "," << x.y << ")";
}