//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_DRIVENEQUATION_H
#define STUDIOSLAB_DRIVENEQUATION_H

#include "Mappings/R2toR/Model/FieldState.h"
#include "Mappings/R2toR/Core/Allocator_R2toR.h"
#include "Phys/DifferentialEquations/DifferentialEquation.h"
#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"



namespace R2toR {

    typedef Phys::Gordon::GordonSystem<FieldState> DiffEq;


    class DrivenEquation : public DiffEq {
    public:
        explicit DrivenEquation();

        auto dtF(const FieldState &in, FieldState &out, Real t, Real dt) -> FieldState & override;
    };


    class DrivenEqAllocator : public Allocator_R2toR {
    public:
        auto getSystemSolver() -> void * override;
    };

}

#endif //STUDIOSLAB_DRIVENEQUATION_H
