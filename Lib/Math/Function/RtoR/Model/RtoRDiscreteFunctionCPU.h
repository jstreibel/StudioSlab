    //
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRDiscreteFunction.h"

namespace Slab::Math::RtoR {
class DiscreteFunction_CPU : public RtoR::DiscreteFunction {
    public:
        DiscreteFunction_CPU(const DiscreteFunction_CPU& toCopy);
        explicit DiscreteFunction_CPU(const DiscreteFunction& toCopy);
        DiscreteFunction_CPU(UInt N, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);
        DiscreteFunction_CPU(RealArray_I& data, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);

        [[nodiscard]] auto Clone() const -> Function_ptr override;

        [[nodiscard]] auto CloneWithSize(UInt N) const -> Pointer<Base::DiscreteFunction<Real, Real>> override;

        auto Set(const RealArray &vec) -> DiscreteFunction_CPU &;
        auto Set(const FunctionT &func) -> DiscreteFunction_CPU & override;

        auto Apply(const MyBase &func,
                   Base::DiscreteFunction<Real, Real> &out)
                   const -> Base::DiscreteFunction<Real, Real> & override;

        auto Laplacian(DiscreteFunction &outFunc) const -> DiscreteFunction & override;

        Base::DiscreteFunction<Real, Real> &operator+=(const MyBase &func) override;

        Real integrate() const override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
