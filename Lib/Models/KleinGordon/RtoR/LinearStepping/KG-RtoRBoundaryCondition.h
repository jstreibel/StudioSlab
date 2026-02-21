//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_RTORBOUNDARYCONDITION_H
#define V_SHAPE_RTORBOUNDARYCONDITION_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "KG-RtoREquationState.h"

#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class FBoundaryCondition : public Base::BoundaryConditions {
        RtoR::Function_ptr initialPhiCondition;
        RtoR::Function_ptr leftPhiBoundaryCondition;
        RtoR::Function_ptr rightPhiBoundaryCondition;
        RtoR::Function_ptr initialdPhiDtCondition;
        RtoR::Function_ptr leftdPhiDtBoundaryCondition;
        RtoR::Function_ptr rightdPhiDtBoundaryCondition;

    protected:

        virtual void ApplyKG(EquationState &kgState, DevFloat t) const;

    public:
        FBoundaryCondition(const KGRtoR::EquationState_constptr &prototype,
                           RtoR::Function_ptr initialPhiCondition,
                           RtoR::Function_ptr initialdPhiDtCondition,
                           RtoR::Function_ptr leftPhiBoundaryCondition = nullptr,
                           RtoR::Function_ptr leftdPhiDtBoundaryCondition = nullptr,
                           RtoR::Function_ptr rightPhiBoundaryCondition = nullptr,
                           RtoR::Function_ptr rightdPhiDtBoundaryCondition = nullptr);

        ~FBoundaryCondition() override;

        void Apply(Base::EquationState &state, DevFloat t) const final;
    };

    using BoundaryCondition [[deprecated("Use FBoundaryCondition")]] = FBoundaryCondition;

    DefinePointers(FBoundaryCondition)
}


#endif //V_SHAPE_RTORBOUNDARYCONDITION_H
