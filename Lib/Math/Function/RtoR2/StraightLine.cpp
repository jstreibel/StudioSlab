//
// Created by joao on 15/10/2019.
//

#include "StraightLine.h"

#include <memory>

RtoR2::StraightLine::StraightLine(Real2D x0, Real2D xf, Real sMin, Real sMax)
    : ParametricCurve(sMin, sMax), x0(x0), r(xf-x0) {   }

RtoR2::StraightLine::StraightLine(const RtoR2::StraightLine &line)
    : StraightLine(line.x0, line.x0+line.r, line.get_sMin(), line.get_sMax()) {   }

Real2D RtoR2::StraightLine::operator()(Real s) const {
    return x0 + r * normalize(s);
}

RtoR2::StraightLine &RtoR2::StraightLine::operator=(const RtoR2::StraightLine &rhs) {
    x0 = rhs.x0;
    r = rhs.r;

    set_s(rhs.get_sMin(), rhs.get_sMax());

    return *this;
}

RtoR2::StraightLine operator*(const Transform &T, const RtoR2::StraightLine &line) {
    Real2D x0 = line.getx0();
    Real2D xf = x0 + line.getr();

    return {T*x0, T*xf, line.get_sMin(), line.get_sMax()};
}

RtoR2::StraightLine::Ptr RtoR2::StraightLine::New(Real2D x0, Real2D xf) {
    return std::make_shared<RtoR2::StraightLine>(x0, xf);
}

Math::PointSet::Ptr
RtoR2::StraightLine::renderToPointSet(RenderingOptions options) {
    auto set = Math::PointSet({x0, x0 + r});

    return std::make_shared<Math::PointSet>(set);
}


