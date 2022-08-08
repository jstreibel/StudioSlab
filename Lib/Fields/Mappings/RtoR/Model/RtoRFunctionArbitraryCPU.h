//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRFunctionArbitrary.h"

namespace RtoR {
    class FunctionArbitraryCPU : public ArbitraryFunction {
    public:
        FunctionArbitraryCPU(const FunctionArbitraryCPU& toCopy);
        FunctionArbitraryCPU(PosInt N, Real xLeft, Real xRight, ArbitraryFunction::LaplacianType laplacianType);

        [[nodiscard]] auto Clone() const -> Base::Function<Real, Real> * override;

        [[nodiscard]] auto CloneWithSize(PosInt N) const -> Base::ArbitraryFunction<Real, Real> * override;

        auto Set(const VecFloat &vec) -> FunctionArbitraryCPU &;
        auto Set(const Function &func) -> FunctionArbitraryCPU & override;

        auto Apply(const MyBase &func, Base::ArbitraryFunction<Real, Real> &out) const -> Base::ArbitraryFunction<Real, Real> & override;

        auto Laplacian(ArbitraryFunction &outFunc) const -> ArbitraryFunction & override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
