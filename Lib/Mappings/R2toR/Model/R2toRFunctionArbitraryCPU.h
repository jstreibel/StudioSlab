//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "R2toRFunctionArbitrary.h"
#include "R2toRFunction.h"

namespace R2toR {
    class FunctionArbitraryCPU : public FunctionArbitrary {
    public:
        FunctionArbitraryCPU(PosInt N, PosInt M, Real xMin, Real yMin, Real h);

        [[nodiscard]] Base::ArbitraryFunction<Real2D, Real> *CloneWithSize(PosInt N) const override;

        FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const override;

        Real At(PosInt n, PosInt m) const override;

        Real &At(PosInt n, PosInt m);


        FunctionArbitraryCPU &Set(const R2toR::Function &func) override;


        ArbitraryFunction <Real2D, Real> &
        Apply(const Function<Real, Real> &func, ArbitraryFunction <Real2D, Real> &out) const override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
