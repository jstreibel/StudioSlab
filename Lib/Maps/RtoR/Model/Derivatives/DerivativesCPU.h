#ifndef DERIVATIVES_H
#define DERIVATIVES_H


#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"


namespace RtoR {

    class DerivativeCPU : public RtoR::Function {

        auto dfdx_fixed_3s(const UInt &X) const -> Real;
        auto dfdx_fixed_5s(const UInt &X) const -> Real;
        auto dfdx_periodic_3s(const UInt &X) const -> Real;
        auto dfdx_loose_3s(UInt X) const -> Real;

        auto d2fdx2_fixed_3s(const UInt &X) const -> Real;
        auto d2fdx2_fixed_5s(UInt X) const -> Real;
        auto d2fdx2_periodic_3s(const UInt &X) const -> Real;

        const RealArray *f_;
        const Real h, inv12h, inv2h, invhsqr, inv12hsqr;
        const UInt N;

        bool periodic;

    public:
        explicit DerivativeCPU(const DiscreteFunction &in);

        void changeFunc(const DiscreteFunction &vec) {
            assert(vec.getSpace().getTotalDiscreteSites() == N);
            assert(vec.getSpace().geth() == h);

            f_ = &vec.getSpace().getHostData();
        }

        auto dfdx_v(RealArray_O &out) -> RealArray &;
        auto d2fdx2_v(RealArray_O &out) -> RealArray &;

        Real operator()(Real x) const override;
    };
}

#endif
