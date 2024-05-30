//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_RTORBOUNDARYCONDITION_H
#define V_SHAPE_RTORBOUNDARYCONDITION_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/Numerics/Solver/BoundaryConditions.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class BoundaryCondition : public Base::BoundaryConditions {
        RtoR::Function_ptr initialPhiCondition;
        RtoR::Function_ptr leftPhiBoundaryCondition;
        RtoR::Function_ptr rightPhiBoundaryCondition;
        RtoR::Function_ptr initialdPhiDtCondition;
        RtoR::Function_ptr leftdPhiDtBoundaryCondition;
        RtoR::Function_ptr rightdPhiDtBoundaryCondition;

    protected:

        virtual void applyKG(EquationState &kgState, Real t) const;

    public:
        BoundaryCondition(const KGRtoR::EquationState_constptr &prototype,
                          RtoR::Function_ptr initialPhiCondition,
                          RtoR::Function_ptr initialdPhiDtCondition,
                          RtoR::Function_ptr leftPhiBoundaryCondition = nullptr,
                          RtoR::Function_ptr leftdPhiDtBoundaryCondition = nullptr,
                          RtoR::Function_ptr rightPhiBoundaryCondition = nullptr,
                          RtoR::Function_ptr rightdPhiDtBoundaryCondition = nullptr);

        virtual ~BoundaryCondition();

        void apply(Base::EquationState &state, Real t) const final;



    };

    DefinePointer(BoundaryCondition)
}


#endif //V_SHAPE_RTORBOUNDARYCONDITION_H
