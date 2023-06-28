#ifndef DERIVATIVES_H
#define DERIVATIVES_H


#include "Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"


namespace RtoR {

class DerivativeCPU : public RtoR::Function {
    public:
        explicit DerivativeCPU(const ArbitraryFunction &in);

        void changeFunc(const ArbitraryFunction &vec) {
            assert(vec.getSpace().getTotalDiscreteSites() == N);
            assert(vec.getSpace().geth() == h);

            f_ = &vec.getSpace().getHostData();
        }

        auto d2fdx2_v(VecFloat_O &out) -> VecFloat &;

        Real operator()(Real x) const override;

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
