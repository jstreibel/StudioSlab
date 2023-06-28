//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "R2toRDiscreteFunction.h"
#include "R2toRFunction.h"

namespace R2toR {
    class FunctionArbitraryCPU : public DiscreteFunction {
    public:
        FunctionArbitraryCPU(PosInt N, PosInt M, Real xMin, Real yMin, Real h);

        [[nodiscard]] Base::DiscreteFunction<Real2D, Real> *CloneWithSize(PosInt N) const override;

        DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const override;

        Real At(PosInt n, PosInt m) const override;

        Real &At(PosInt n, PosInt m);

        FunctionArbitraryCPU &Set(const R2toR::Function &func) override;


        auto Apply(const FuncBase &func, DiscrBase &out) const -> DiscrBase & override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H