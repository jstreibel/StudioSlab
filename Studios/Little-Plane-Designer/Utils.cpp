//
// Created by joao on 11/28/25.
//

#include "Utils.h"

#include "utils/enums.h"

b2Vec2 b2Vec2_FromPoint2D(Slab::Math::Point2D Pt) {
    return {static_cast<float>(Pt.x), static_cast<float>(Pt.y)};
}

Slab::Math::Point2D Point2D_Fromb2Vec2(b2Vec2 Vec) {
    return {static_cast<double>(Vec.x), static_cast<double>(Vec.y)};
}