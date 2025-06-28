#ifndef DERIVATIVES_H
#define DERIVATIVES_H


#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"


namespace Slab::Math::RtoR {

    class DerivativeCPU : public RtoR::Function {

        auto dfdx_fixed_3s(const UInt &X) const -> DevFloat;
        auto dfdx_fixed_5s(const UInt &X) const -> DevFloat;
        auto dfdx_periodic_3s(const UInt &X) const -> DevFloat;
        auto dfdx_loose_3s(UInt X) const -> DevFloat;

        auto d2fdx2_fixed_3s(const UInt &X) const -> DevFloat;
        auto d2fdx2_fixed_5s(UInt X) const -> DevFloat;
        auto d2fdx2_periodic_3s(const UInt &X) const -> DevFloat;

        const RealArray *f_;
        const DevFloat h, inv12h, inv2h, invhsqr, inv12hsqr;
        const UInt N;

        bool periodic;

    public:
        explicit DerivativeCPU(const NumericFunction &in);

        void changeFunc(const NumericFunction &vec) {
            assert(vec.getSpace().getTotalDiscreteSites() == N);
            assert(vec.getSpace().getMetaData().geth(0) == h);

            f_ = &vec.getSpace().getHostData();
        }

        auto dfdx_v(RealArray_O &out) -> RealArray &;
        auto d2fdx2_v(RealArray_O &out) -> RealArray &;

        DevFloat operator()(DevFloat x) const override;
    };
}

#endif
