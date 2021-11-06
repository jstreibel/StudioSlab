//
// Created by joao on 09/09/2019.
//

#include "AnalyticOscillon.h"

#include "analytic-breather.h"
#include <Phys/Category/Categories.h>
#include <cmath>

using namespace RtoR;

auto gamma(Real u) -> Real{
    return 1./sqrt(1.-u*u);
}

auto ab(Real alpha, Real u, Real v) -> RealPair{
    const Real alpha_0 = .5*(1.-u*u);

    assert(!(alpha<0. || alpha>1.));

    if(alpha < alpha_0) return {1., 0.};

    return {-1., 1.};
}

auto x_0(Real alpha, Real u, Real v) -> Real{
    RealPair p_ab = ab(alpha, u, v);
    Real a = p_ab.first;
    Real b = p_ab.second;

    return -gamma(u)/(1.+a*v*u) * ((1.-u*u)*v*b + alpha*(u+v*a));
}

auto t_0(Real alpha, Real u) -> Real{
    return gamma(u)*alpha;
}

AnalyticOscillon::AnalyticOscillon(const AnalyticOscillon &copy) :
    t0(copy.t0), x0(copy.x0), l(copy.l), v(copy.v), u(copy.u), s1(copy.s1), s2(copy.s2), bit(copy.bit) {
}

AnalyticOscillon::AnalyticOscillon(Real t, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, Bit bit)
        : t0(t_0(alpha, -u)), x0(x_0(alpha, -u, v)), l(1), v(v), u(u), s1(phiMirrored?-1:1), s2(xMirrored?-1:1), bit(bit) {

}

AnalyticOscillon::AnalyticOscillon(Real t, Real x0, Real l, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, AnalyticOscillon::Bit bit)
        : t0(t_0(alpha, -u)-t), x0(x_0(alpha, -u, v)+x0), l(l), v(v), u(u), s1(phiMirrored?-1:1), s2(xMirrored?-1:1), bit(bit) {

}

auto AnalyticOscillon::operator()(Real x) const -> Real {
    if(bit==dPhiDt) return s1*l*dpsi(t0/l, (s2*x-x0)/l, v, -u);

    return s1*l*l*psi(t0/l, (s2*x-x0)/l, v, -u);
}

auto AnalyticOscillon::Clone() const -> Function * {
    return new AnalyticOscillon(*this);
}

auto AnalyticOscillon::swap() -> AnalyticOscillon & {
    bit = (bit == phi ? dPhiDt : phi);

    return *this;
}


