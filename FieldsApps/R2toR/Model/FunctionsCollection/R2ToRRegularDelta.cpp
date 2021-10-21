//
// Created by joao on 18/12/2019.
//

#include "R2ToRRegularDelta.h"

Real R2toR::R2toRRegularDelta::operator()(Real2D x) const {
    const float r = x.norm();

    if(r < eps) return a*(3./(pi*eps*eps)) * (1. - r/eps);

    return .0;
}
