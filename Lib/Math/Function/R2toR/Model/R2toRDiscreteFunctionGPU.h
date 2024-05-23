//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
#define V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H

#include "R2toRDiscreteFunction.h"


#if USE_CUDA
namespace Slab::Math::R2toR {

    class DiscreteFunction_GPU : public R2toR::DiscreteFunction {
        DiscreteFunction_GPU *helper= nullptr;

    public:
        DiscreteFunction_GPU(UInt N, UInt M, Real xMin, Real yMin, Real h);
        DiscreteFunction_GPU(UInt N, Real sMin, Real h);
        ~DiscreteFunction_GPU() override;

        auto Laplacian        (DiscreteFunction &outFunc)            const -> DiscreteFunction & override;

        auto Clone()                                                 const -> Type             * override;
        auto CloneWithSize    (UInt N)                             const -> DiscrBase        * override;

        auto Set              (const MyBase &func)                         -> DiscreteFunction & override;
        auto SetArb           (const DiscreteFunctionBase &func)           -> DiscreteFunction & override;

        auto Apply(const FuncBase &func, DiscrBase &out)             const -> DiscrBase        & override;

        auto Add              (const DiscrBase & toi)                      -> DiscreteFunction & override;
        auto StoreAddition    (const DiscrBase &, const DiscrBase&)        -> DiscreteFunction & override;
        auto StoreSubtraction (const DiscrBase &, const DiscrBase&)        -> DiscreteFunction & override;
        auto Multiply         (floatt a)                                   -> DiscreteFunction & override;

        auto operator+=(const MyBase &func)                                -> DiscrBase        & override;

        auto At               (UInt n, UInt m)                   const -> Real               override;
        auto At               (UInt n, UInt m)                         -> Real             & override;

    };

}
#endif

#endif //V_SHAPE_R2TORFUNCTIONARBITRARYGPU_H
