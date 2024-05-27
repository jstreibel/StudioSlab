//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H

#include "Utils/Numbers.h"

#include "R2toRDiscreteFunction.h"
#include "R2toRFunction.h"



namespace Slab::Math::R2toR {

    typedef floatt Real;

    class DiscreteFunction_CPU : public DiscreteFunction {
    public:
        DiscreteFunction_CPU(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy);

        [[nodiscard]] Pointer<Base::DiscreteFunction<Real2D, Real>> CloneWithSize(UInt N) const override;

        auto Clone() const -> FunctionT * override;

        DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const override;

        Real At(UInt n, UInt m) const override;

        Real &At(UInt n, UInt m) override;

        DiscreteFunction_CPU &Set(const R2toR::Function &func) override;



        auto Apply(const FuncBase &func, DiscrBase &out) const -> DiscrBase & override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARYCPU_H
