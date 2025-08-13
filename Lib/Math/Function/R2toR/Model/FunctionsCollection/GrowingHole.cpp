//
// Created by joao on 10/11/22.
//

#include "GrowingHole.h"

namespace Slab::Math::R2toR {

    GrowingHoleFunc::GrowingHoleFunc(DevFloat height, DevFloat t) : t(t), height(height) {

    }

    DevFloat GrowingHoleFunc::operator()(Real2D x) const {
        auto r = x.norm();

        assert(r >= t);

        return height;
    }


    void GrowingHoleFunc::setTime(DevFloat t) {
        this->t = t;
    }

    bool GrowingHoleFunc::domainContainsPoint(Real2D x) const {
        return x.norm() >= t;
    }

}