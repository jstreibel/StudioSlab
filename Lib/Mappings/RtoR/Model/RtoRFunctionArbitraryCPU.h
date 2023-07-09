//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRFunctionArbitrary.h"

namespace RtoR {
    class FunctionArbitraryCPU : public DiscreteFunction {
    public:
        FunctionArbitraryCPU(const FunctionArbitraryCPU& toCopy);
        FunctionArbitraryCPU(const DiscreteFunction& toCopy);
        FunctionArbitraryCPU(PosInt N, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D);
        FunctionArbitraryCPU(VecFloat_I data, Real xLeft, Real xRight,
                             DiscreteFunction::LaplacianType laplacianType=LaplacianType::Standard1D);

        [[nodiscard]] auto Clone() const -> Base::Function<Real, Real> * override;

        [[nodiscard]] auto CloneWithSize(PosInt N) const -> Base::DiscreteFunction<Real, Real> * override;

        auto Set(const VecFloat &vec) -> FunctionArbitraryCPU &;
        auto Set(const Function &func) -> FunctionArbitraryCPU & override;

        auto Apply(const MyBase &func,
                   Base::DiscreteFunction<Real, Real> &out)
                   const -> Base::DiscreteFunction<Real, Real> & override;

        auto Laplacian(DiscreteFunction &outFunc) const -> DiscreteFunction & override;

        Real integrate() const override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
