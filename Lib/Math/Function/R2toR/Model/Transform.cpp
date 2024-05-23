//
// Created by joao on 17/10/2019.
//

#include "Transform.h"


namespace Slab::Math::R2toR {


    Rotation::Rotation(Real angleRad) : angle(angleRad) {

    }

    Real2D Rotation::operator*(const Real2D &x) const {
        const Real c = cos(angle), s = sin(angle);

        return {c * x.x - s * x.y, s * x.x + c * x.y};
    }

    Rotation Rotation::ByAngle(Real angleRad) {
        return Rotation(angleRad);
    }


    Translation::Translation(Real2D a) : a(a) {}

    Real2D Translation::operator*(const Real2D &x) const {
        return x + a;
    }


    Scale::Scale(const Real2D &s) : s(s) {}

    Real2D Scale::operator*(const Real2D &x) const {
        return {x.x * s.x, x.y * s.y};
    }

}