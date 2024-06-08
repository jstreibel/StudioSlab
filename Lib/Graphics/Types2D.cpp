//
// Created by joao on 1/10/23.
//

#include "Types2D.h"

namespace Slab::Graphics {

    // Point2D operator+(const Point2D &a, const Point2D &b) { return {a.x + b.x, a.y + b.y}; }

    Point2D operator-(const Point2D &a, const Point2D &b) { return {a.x - b.x, a.y - b.y}; }

    Point2D operator*(const Real &a, const Point2D &p) { return {p.x * a, p.y * a}; }

}