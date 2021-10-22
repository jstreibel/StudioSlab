//
// Created by joao on 17/10/2019.
//

#include "Transform.h"

Rotation::Rotation(Real angleRad) : angle(angleRad) {

}

Real2D Rotation::operator*(const Real2D &x) const {
    const Real c = cos(angle), s = sin(angle);

    return {c*x.x-s*x.y, s*x.x+c*x.y};
}

Rotation Rotation::ByAngle(Real angleRad) {
    return Rotation(angleRad);
}
