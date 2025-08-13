//

#include "DerivativesCPU.h"

#include "Utils/OMPUtils.h"

//
// Created by joao on 09/09/2019.

namespace Slab::Math::RtoR {

#define RESOLVE_BC (in.getLaplacianType()==NumericFunction::Standard1D_PeriodicBorder ? true : false)

    DerivativeCPU::DerivativeCPU(const NumericFunction &in)
            : f_(&in.getSpace().getHostData()), h(in.getSpace().getMetaData().geth(0)), inv2h(.5 / h),
              inv12h((1. / 12) / h), invhsqr(1. / (h * h)), inv12hsqr((1. / 12) / (h * h)),
              N(in.getSpace().getTotalDiscreteSites()), periodic(RESOLVE_BC) {
    }

    auto DerivativeCPU::dfdx_periodic_3s(const UInt &i) const -> DevFloat {
        const RealArray &f = *f_;

        fix LEFT = (i > 0) ? i - 1 : N - 1,
                RIGHT = (i < N - 1) ? i + 1 : 0;

        return (f[RIGHT] - f[LEFT]) * inv2h;
    }

    auto DerivativeCPU::dfdx_fixed_3s(const UInt &i) const -> DevFloat {

        if (i < 1 || i > N - 2) return 0;

        const RealArray &f = *f_;

        fix LEFT = i - 1,
                RIGHT = i + 1;

        return (f[RIGHT] - f[LEFT]) * inv2h;
    }

    auto DerivativeCPU::dfdx_fixed_5s(const UInt &X) const -> DevFloat {
        const size_t &i = X;
        const RealArray &f = *f_;

        if (X == 0) return 0.;
        else if (X == 1) return 0.;
        else if (X == N - 1) return 0.;
        else if (X == N - 2) return 0.;

        // 1 -8 0 8 -1  /  12
        return ((f[i - 2] - f[i + 2]) - 8 * (f[i - 1] - f[i + 1])) * inv12h;
    }

    auto DerivativeCPU::dfdx_loose_3s(UInt X) const -> DevFloat {
        const size_t &i = X;
        const RealArray &f = *f_;

        if (X == 0) return inv2h * (-3 * f[i] + 4 * f[i + 1] - f[i + 2]);
        else if (X == N - 1) return inv2h * (+3 * f[i] - 4 * f[i - 1] + f[i - 2]);

        return inv2h * (-f[i - 1] + f[i + 1]);
    }

    auto DerivativeCPU::d2fdx2_fixed_3s(const UInt &i) const -> DevFloat {
        const RealArray &f = *f_;

        if (i == 0) { return 0; }
        else if (i == N - 1) return 0;

        const auto LEFT = i - 1, RIGHT = i + 1, CENTER = i;

        return (i < 1 || i > N - 2) ? 0 : invhsqr * ((f[LEFT] + f[RIGHT]) - 2.0 * f[CENTER]);
    }

    auto DerivativeCPU::d2fdx2_fixed_5s(UInt X) const -> DevFloat {
        const size_t &i = X;
        const RealArray &f = *f_;

        if (X < 2 || X > N - 3) return 0;

        return inv12hsqr * ((-(f[i - 2] + f[i + 2]) + 16 * (f[i - 1] + f[i + 1])) - 30 * f[i]);
    }

    auto DerivativeCPU::d2fdx2_periodic_3s(const UInt &i) const -> DevFloat {
        const RealArray &f = *f_;

        auto LEFT = (i > 0) ? i - 1 : N - 1,
                CENTER = i,
                RIGHT = (i < N - 1) ? i + 1 : 0;

        return invhsqr * ((f[LEFT] + f[RIGHT]) - 2.0 * f[CENTER]);
    }

    auto DerivativeCPU::d2fdx2_v(RealArray_O &d2fdx2_O) -> RealArray & {

        if (periodic) {
            OMP_PARALLEL_FOR(n, N)d2fdx2_O[n] = d2fdx2_periodic_3s(n);
        } else {
            OMP_PARALLEL_FOR(n, N)d2fdx2_O[n] = d2fdx2_fixed_3s(n);
        }

        return d2fdx2_O;
    }

    auto DerivativeCPU::dfdx_v(RealArray_O &dfdx_O) -> RealArray & {
        if (periodic) {
            OMP_PARALLEL_FOR(n, N)dfdx_O[n] = dfdx_periodic_3s(n);
        } else {
            OMP_PARALLEL_FOR(n, N)dfdx_O[n] = dfdx_fixed_3s(n);
        }

        return dfdx_O;
    }

    DevFloat DerivativeCPU::operator()(DevFloat x) const {
        throw "DerivativeCPU::operator() not implemented.";
    }

}