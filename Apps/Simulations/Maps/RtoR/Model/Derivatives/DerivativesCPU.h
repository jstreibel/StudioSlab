#ifndef DERIVATIVES_H
#define DERIVATIVES_H

#include "Model/nr3.h"
#include "Model/math-util.h"

#include "Model/MathTypes.h"

#include "Apps/Simulations/Maps/RtoR/Model/RtoRFunctionArbitraryCPU.h"

#include <omp.h>

namespace RtoR {

    class DerivativeCPU {
    public:
        explicit DerivativeCPU(const FunctionArbitrary &in);

        void changeFunc(const FunctionArbitrary &vec) {
            assert(vec.getSpace().getTotalDiscreteSites() == N);
            assert(vec.getSpace().geth() == h);

            f_ = &vec.getSpace().getX();
        }

        auto d2fdx2_v(VecFloat_O &out) -> VecFloat &;

    private:

        auto dfdx(PosInt X) const -> Real;
        auto d2fdx2(PosInt X) const -> Real;

        auto dfdx_3s(PosInt X) const -> Real;
        auto dfdx_5s(PosInt X) const -> Real;

        auto d2fdx2_3s(PosInt X) const -> Real;
        auto d2fdx2_5s(PosInt X) const -> Real;

        const VecFloat *f_;
        const Real h, invh, invhsqr, inv12 = 1.0 / 12.0;
        const PosInt N;
    };
}

#endif
