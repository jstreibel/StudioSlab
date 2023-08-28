//
// Created by joao on 9/08/23.
//


#include "Types.h"



Point2D operator + (const Point2D &a, const Point2D &b){ return {a.x+b.x, a.y+b.y}; }
Point2D operator - (const Point2D &a, const Point2D &b){ return {a.x-b.x, a.y-b.y}; }
Point2D operator * (const Real &a, const Point2D &p){ return {p.x*a, p.y*a}; }

