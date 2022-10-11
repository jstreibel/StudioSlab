//
// Created by joao on 10/11/22.
//

#include "GrowingHole.h"

GrowingHole::GrowingHole(Real height, Real t) : t(t), height(height) {

}

Real GrowingHole::operator()(Real2D x) const {
    auto r = x.norm();

    assert(r>=t);

    return height;
}


void GrowingHole::setTime(Real t) {
    this->t = t;
}

bool GrowingHole::domainContainsPoint(Real2D x) const {
    return x.norm() >= t;
}
