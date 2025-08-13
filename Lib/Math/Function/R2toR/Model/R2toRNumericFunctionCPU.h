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

    typedef floatt Real;

    class NumericFunction_CPU : public FNumericFunction {
    public:
        NumericFunction_CPU(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy);

        [[nodiscard]]
        TPointer<Base::NumericFunction<Real2D, Real>>
        CloneWithSize(UInt N) const override;

        auto
        Clone()
        const -> Function_ptr override;

        FNumericFunction &
        Laplacian(FNumericFunction &outFunc) const override;

        const Real &
        At(UInt n, UInt m) const override;

        Real &
        At(UInt n, UInt m) override;

        NumericFunction_CPU &
        Set(const R2toR::Function &func) override;

        auto
        Apply(const FuncBase &func, DiscrBase &out)
        const -> DiscrBase & override;
    };

}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
