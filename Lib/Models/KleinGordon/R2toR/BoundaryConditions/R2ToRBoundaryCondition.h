//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORBOUNDARYCONDITION_H
#define V_SHAPE_R2TORBOUNDARYCONDITION_H

#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"


namespace Slab::Math::R2toR {

    class FBoundaryCondition : public Base::BoundaryConditions {
        Function *initialPhiCondition;
        Function *initialdPhiDtCondition;

    public:
        FBoundaryCondition(Function *initialPhiCondition,
                           Function *initialdPhiDtCondition,
                           const R2toR::EquationState_constptr& prototype);



        void Apply(Base::EquationState &toFunction, DevFloat t) const final;

    protected:
        virtual void apply_KGR2toR(EquationState &fieldState, DevFloat t) const;
    };

    using BoundaryCondition [[deprecated("Use FBoundaryCondition")]] = FBoundaryCondition;
}


#endif //V_SHAPE_R2TORBOUNDARYCONDITION_H
