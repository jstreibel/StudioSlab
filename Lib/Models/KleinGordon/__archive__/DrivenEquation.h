//
// Created by joao on 08/06/23.
//

#ifndef STUDIOSLAB_DRIVENEQUATION_H
#define STUDIOSLAB_DRIVENEQUATION_H

#include "Models/KleinGordon/R2toR/EquationState.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunctionGPU.h"
#include "Math/DifferentialEquations/EquationSolver.h"
#include "Models/KleinGordon/KGSolver.h"
#include "../../../../Studios/Fields/R2toR/LeadingDelta/RingDeltaFunc.h"

namespace R2toR {

    namespace LeadingDelta {

        typedef Phys::Gordon::GordonSolverT<EquationState> DiffEq;

        class DrivenEquation : public DiffEq {
            R2toR::Function::Ptr drivingForce;
            R2toR::FunctionArbitraryGPU drivingForceRendered;

        public:
            explicit DrivenEquation(Base::Simulation::VoidBuilder &builder, R2toR::Function::Ptr drivingForce);

            auto dtF(const EquationState &in, EquationState &out, Real t, Real dt) -> EquationState & override;
        };

    }

}

#endif //STUDIOSLAB_DRIVENEQUATION_H
