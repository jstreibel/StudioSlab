//

#include "DerivativesCPU.h"

#include "Utils/OMPUtils.h"

//
// Created by joao on 09/09/2019.

using namespace RtoR;

DerivativeCPU::DerivativeCPU(const DiscreteFunction &in)
: f_(&in.getSpace().getHostData()), h(in.getSpace().geth()), invh(1. / h), invhsqr(1. / (h * h)),
  N(in.getSpace().getTotalDiscreteSites())
{
}

auto DerivativeCPU::dfdx_3s(PosInt X) const -> Real {
    const size_t &i = X;
    const RealVector &f = *f_;

    if(X==0)        return invh * 0.5 * (-3*f[i  ] + 4*f[i+1]    -f[i+2]);
    else if(X==N-1) return invh * 0.5 * (  +f[i-2] - 4*f[i-1] + 3*f[i  ]);

    return                 invh * 0.5 * (  -f[i-1]            +   f[i+1]);
}

auto DerivativeCPU::d2fdx2_3s(PosInt X) const -> Real {
    const size_t &i = X;
    const RealVector &f = *f_;

    #ifdef PERIODIC_BC
    auto LEFT=i-1, RIGHT=i+1, CENTER=i;
    if (i == 0) { LEFT=N-1; }
    else if (i == N - 1) RIGHT=0;
    #else
    if (X == 0) { return 0; }
    else if (X == N - 1) return 0;
    const auto LEFT=i-1, RIGHT=i+1, CENTER=i;
    #endif

    return invhsqr * ((f[LEFT] + f[RIGHT]) - 2.0*f[CENTER]);
}

auto DerivativeCPU::dfdx_5s(PosInt X) const -> Real {
    const size_t &i = X;
    const RealVector &f = *f_;

    if(X==0)        return 0.;
    else if(X==1)   return 0.;
    else if(X==N-1) return 0.;
    else if(X==N-2) return 0.;

    // 1 -8 0 8 -1  /  12
    return ((f[i-2]-f[i+2])-8*(f[i-1] - f[i+1])) * invh * inv12;
}

auto DerivativeCPU::d2fdx2_5s(PosInt X) const -> Real {
    const size_t &i = X;
    const RealVector &f = *f_;

    if(X<2 || X > N-3) return 0;

    return     invhsqr * inv12 * ((-(f[i-2]+f[i+2]) + 16*(f[i-1]+f[i+1])) -  30*f[i]);
}

auto DerivativeCPU::dfdx(PosInt X) const -> Real {
    return dfdx_3s(X);
}

auto DerivativeCPU::d2fdx2(PosInt X) const -> Real {
    return d2fdx2_3s(X);
}

auto DerivativeCPU::d2fdx2_v(VecFloat_O &d2fdx2_O) -> RealVector & {

    OMP_PARALLEL_FOR(n, N)
        d2fdx2_O[n] = d2fdx2(n);

    return d2fdx2_O;
}

Real DerivativeCPU::operator()(Real x) const {
    throw "DerivativeCPU::operator() not implemented.";
}
