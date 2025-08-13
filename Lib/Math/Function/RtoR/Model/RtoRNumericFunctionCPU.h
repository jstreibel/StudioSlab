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
        NumericFunction_CPU(UInt N, DevFloat xLeft, DevFloat xRight,
                             NumericFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);
        NumericFunction_CPU(RealArray_I& data, DevFloat xLeft, DevFloat xRight,
                             NumericFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);

        [[nodiscard]] auto Clone() const -> Function_ptr override;

        [[nodiscard]] auto CloneWithSize(UInt N) const -> TPointer<Base::NumericFunction<DevFloat, DevFloat>> override;

    DevFloat max() const override;

    DevFloat min() const override;

    auto Set(const RealArray &vec) -> NumericFunction_CPU &;
        auto Set(const FunctionT &func) -> NumericFunction_CPU & override;

        auto Apply(const MyBase &func,
                   Base::NumericFunction<DevFloat, DevFloat> &out)
                   const -> Base::NumericFunction<DevFloat, DevFloat> & override;

        auto Laplacian(NumericFunction &outFunc) const -> NumericFunction & override;

        Base::NumericFunction<DevFloat, DevFloat> &operator+=(const MyBase &func) override;

        DevFloat integrate() const override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
