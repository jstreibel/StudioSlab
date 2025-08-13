//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "Utils/Numbers.h"

#include "R2toRNumericFunction.h"
#include "R2toRFunction.h"
#include "Math/Data/DataAllocator.h"


namespace Slab::Math::R2toR {

    typedef floatt DevFloat;

    class NumericFunction_CPU : public FNumericFunction {
    public:
        NumericFunction_CPU(UInt N, UInt M, DevFloat xMin, DevFloat yMin, DevFloat hx, DevFloat hy);

        [[nodiscard]]
        TPointer<Base::NumericFunction<Real2D, DevFloat>>
        CloneWithSize(UInt N) const override;

        auto
        Clone()
        const -> Function_ptr override;

        FNumericFunction &
        Laplacian(FNumericFunction &outFunc) const override;

        const DevFloat &
        At(UInt n, UInt m) const override;

        DevFloat &
        At(UInt n, UInt m) override;

        NumericFunction_CPU &
        Set(const R2toR::Function &func) override;

        auto
        Apply(const FuncBase &func, DiscrBase &out)
        const -> DiscrBase & override;
    };

}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
