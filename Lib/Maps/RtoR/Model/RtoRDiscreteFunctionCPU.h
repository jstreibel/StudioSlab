//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRDiscreteFunction.h"

namespace RtoR {
    class DiscreteFunction_CPU : public DiscreteFunction {
    public:
        DiscreteFunction_CPU(const DiscreteFunction_CPU& toCopy);
        explicit DiscreteFunction_CPU(const DiscreteFunction& toCopy);
        DiscreteFunction_CPU(UInt N, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);
        DiscreteFunction_CPU(RealArray_I& data, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D_FixedBorder);

        [[nodiscard]] auto Clone() const -> Core::FunctionT<Real, Real> * override;

        [[nodiscard]] auto CloneWithSize(UInt N) const -> Core::DiscreteFunction<Real, Real> * override;

        auto Set(const RealArray &vec) -> DiscreteFunction_CPU &;
        auto Set(const FunctionT &func) -> DiscreteFunction_CPU & override;

        auto Apply(const MyBase &func,
                   Core::DiscreteFunction<Real, Real> &out)
                   const -> Core::DiscreteFunction<Real, Real> & override;

        auto Laplacian(DiscreteFunction &outFunc) const -> DiscreteFunction & override;

        Real integrate() const override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
