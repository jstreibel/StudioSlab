//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORBOUNDARYCONDITION_H
#define V_SHAPE_R2TORBOUNDARYCONDITION_H

#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Math/Numerics/Solver/BoundaryConditions.h"


namespace Slab::Math::R2toR {

    class BoundaryCondition : public Base::BoundaryConditions {
        Function *initialPhiCondition;
        Function *initialdPhiDtCondition;

    public:
        BoundaryCondition(Function *initialPhiCondition,
                          Function *initialdPhiDtCondition,
                          const R2toR::EquationState_constptr& prototype);



        void apply(Base::EquationState &toFunction, Real t) const final;

    protected:
        virtual void apply_KGR2toR(EquationState &fieldState, Real t) const;
    };

    DefinePointer(BoundaryCondition)

}


#endif //V_SHAPE_R2TORBOUNDARYCONDITION_H
