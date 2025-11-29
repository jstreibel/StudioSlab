//
// Created by joao on 11/28/25.
//

#ifndef STUDIOSLAB_UTILS_H
#define STUDIOSLAB_UTILS_H
#include "box2d/math_functions.h"
#include "Math/Point.h"

b2Vec2 b2Vec2_FromPoint2D(Slab::Math::Point2D);

Slab::Math::Point2D Point2D_Fromb2Vec2(b2Vec2);

#endif //STUDIOSLAB_UTILS_H