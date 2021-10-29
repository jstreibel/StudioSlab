//
// Created by joao on 17/09/2019.
//

#ifndef V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_RTORFUNCTIONARBITRARYCPU_H

#include "RtoRFunctionArbitrary.h"

namespace RtoR {
    class FunctionArbitraryCPU : public FunctionArbitrary {
    public:
        FunctionArbitraryCPU(const FunctionArbitraryCPU& toCopy);
        FunctionArbitraryCPU(PosInt N, Real xLeft, Real xRight, FunctionArbitrary::LaplacianType laplacianType);

        [[nodiscard]] auto Clone() const -> Base::Function<Real, Real> * override;

        [[nodiscard]] auto CloneWithSize(PosInt N) const -> Base::FunctionArbitrary<Real, Real> * override;

        auto Set(const Function &func) -> FunctionArbitraryCPU & override;

        auto Apply(const MyBase &func, Base::FunctionArbitrary<Real, Real> &out) const -> Base::FunctionArbitrary<Real, Real> & override;

        auto Laplacian(FunctionArbitrary &outFunc) const -> FunctionArbitrary & override;

    };
}


#endif //V_SHAPE_RTORFUNCTIONARBITRARYCPU_H
