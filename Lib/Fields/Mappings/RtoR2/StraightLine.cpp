//
// Created by joao on 15/10/2019.
//

#include "StraightLine.h"

RtoR2::StraightLine::StraightLine(Real2D x0, Real2D xf) : x0(x0), r(xf-x0) {

}

RtoR2::StraightLine::StraightLine(const RtoR2::StraightLine &line) : x0(line.x0), r(line.r) {

}

Real2D RtoR2::StraightLine::operator()(Real s) const {
    return x0 + r*s;
}

RtoR2::StraightLine &RtoR2::StraightLine::operator=(const RtoR2::StraightLine &rhs) {
    x0 = rhs.x0;
    r = rhs.r;
    return *this;
}


