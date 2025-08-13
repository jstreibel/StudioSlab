//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H

#include "R2toRNumericFunction.h"


#if USE_CUDA
namespace Slab::Math::R2toR {

    class NumericFunction_GPU : public R2toR::FNumericFunction {
        NumericFunction_GPU *helper= nullptr;

    public:
        NumericFunction_GPU(UInt N, UInt M, DevFloat xMin, DevFloat yMin, DevFloat hx, DevFloat hy);
        NumericFunction_GPU(UInt N, DevFloat sMin, DevFloat h);
        ~NumericFunction_GPU() override;

        auto Laplacian        (FNumericFunction &outFunc)            const -> FNumericFunction & override;

        auto Clone()                                                 const -> TPointer<Type>     override;
        auto CloneWithSize    (UInt N)                             const -> TPointer<DiscrBase>  override;

        auto Set              (const MyBase &func)                         -> FNumericFunction & override;
        auto SetArb           (const NumericFunctionBase &func)            -> FNumericFunction & override;

        auto Apply(const FuncBase &func, DiscrBase &out)             const -> DiscrBase       & override;

        auto Add              (const DiscrBase & toi)                      -> FNumericFunction & override;
        auto StoreAddition    (const DiscrBase &, const DiscrBase&)        -> FNumericFunction & override;
        auto StoreSubtraction (const DiscrBase &, const DiscrBase&)        -> FNumericFunction & override;
        auto Multiply         (floatt a)                                   -> FNumericFunction & override;

        auto operator+=(const MyBase &func)                                -> DiscrBase        & override;

        auto At (UInt n, UInt m) const -> const DevFloat & override;
        auto At (UInt n, UInt m)       -> DevFloat & override;

    };

}
#endif

#endif //V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
