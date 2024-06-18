    //
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRFunction.h"
#include "RtoRNumericFunction.h"

namespace Slab::Math::RtoR {
class NumericFunction_CPU : public RtoR::NumericFunction {
    public:
        NumericFunction_CPU(const NumericFunction_CPU& toCopy);
        explicit NumericFunction_CPU(const NumericFunction& toCopy);
        NumericFunction_CPU(UInt N, Real xLeft, Real xRight,
                             NumericFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);
        NumericFunction_CPU(RealArray_I& data, Real xLeft, Real xRight,
                             NumericFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);

        [[nodiscard]] auto Clone() const -> Function_ptr override;

        [[nodiscard]] auto CloneWithSize(UInt N) const -> Pointer<Base::NumericFunction<Real, Real>> override;

        auto Set(const RealArray &vec) -> NumericFunction_CPU &;
        auto Set(const FunctionT &func) -> NumericFunction_CPU & override;

        auto Apply(const MyBase &func,
                   Base::NumericFunction<Real, Real> &out)
                   const -> Base::NumericFunction<Real, Real> & override;

        auto Laplacian(NumericFunction &outFunc) const -> NumericFunction & override;

        Base::NumericFunction<Real, Real> &operator+=(const MyBase &func) override;

        Real integrate() const override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
