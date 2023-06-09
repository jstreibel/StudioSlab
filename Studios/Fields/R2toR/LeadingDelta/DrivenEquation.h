//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_DRIVENEQUATION_H
#define STUDIOSLAB_DRIVENEQUATION_H

#include "Mappings/R2toR/Model/FieldState.h"
#include "Mappings/R2toR/Core/R2toR_Allocator.h"
#include "Phys/DifferentialEquations/DifferentialEquation.h"
#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"
#include "RingDelta.h"

namespace R2toR {

    namespace LeadingDelta {

        typedef Phys::Gordon::GordonSystem<FieldState> DiffEq;

        class DrivenEquation : public DiffEq {
            SpecialRingDelta *ringDelta;
            FunctionArbitrary & δᵣ;

            const Real tMax;
        public:
            explicit DrivenEquation(Real ϵ, Real W₀, Real tₘₐₓ);

            auto dtF(const FieldState &in, FieldState &out, Real t, Real dt) -> FieldState & override;
        };

    }

}

#endif //STUDIOSLAB_DRIVENEQUATION_H
