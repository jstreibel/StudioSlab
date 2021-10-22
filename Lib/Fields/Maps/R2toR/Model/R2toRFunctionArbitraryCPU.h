//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "R2toRFunctionArbitrary.h"
#include "Lib/Fields/Maps/RtoR/Model/RtoRFunction.h"

namespace R2toR {
    class FunctionArbitraryCPU : public FunctionArbitrary {
    public:
        FunctionArbitraryCPU(PosInt N, PosInt M, Real xMin, Real yMin, Real h);

        [[nodiscard]] Base::FunctionArbitrary<Real2D, Real> *CloneWithSize(PosInt N) const override;

        FunctionArbitrary &Laplacian(FunctionArbitrary &outFunc) const override;

        Real At(PosInt n, PosInt m) const override;

        Real &At(PosInt n, PosInt m);

        FunctionArbitraryBase &Set(const Function &func) override;

        Base::FunctionArbitrary<Real2D, Real> &SetArb(const Base::FunctionArbitrary<Real2D, Real> &function) override;

        FunctionArbitraryBase &Apply(const RtoR::Function &func, FunctionArbitraryBase &out) const override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
