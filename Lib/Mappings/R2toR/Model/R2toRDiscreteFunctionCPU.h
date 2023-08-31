//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "R2toRDiscreteFunction.h"
#include "R2toRFunction.h"

namespace R2toR {
    class DiscreteFunction_CPU : public DiscreteFunction {
    public:
        DiscreteFunction_CPU(PosInt N, PosInt M, Real xMin, Real yMin, Real h);

        [[nodiscard]] Core::DiscreteFunction<Real2D, Real> *CloneWithSize(PosInt N) const override;

        auto Clone() const -> FunctionT * override;

        DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const override;

        Real At(PosInt n, PosInt m) const override;

        Real &At(PosInt n, PosInt m);

        DiscreteFunction_CPU &Set(const R2toR::Function &func) override;


        auto Apply(const FuncBase &func, DiscrBase &out) const -> DiscrBase & override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
