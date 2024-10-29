//
// Created by joao on 12/08/23.
//

#ifndef STUDIOSLAB_KG_LANGEVINSOLVER_H
#define STUDIOSLAB_KG_LANGEVINSOLVER_H

#include "Models/KleinGordon/KG-Solver.h"
#include "Models/KleinGordon/KG-State.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRSolver.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"

#include "Math/Numerics/NumericalRecipe.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"


#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {

    class KGLangevinSolver : public KGRtoRSolver {
        RtoR::NumericFunction_ptr langevinImpulses = nullptr;
        RtoR::NumericFunction_ptr scaledImpulses   = nullptr;

        Real T=.0;
        Real γ=.0; // dissipation
        Real α=.0; // helper: α=sqrt(2Tγ/dt);

        void ComputeImpulses();
    public:
        KGLangevinSolver(Base::BoundaryConditions_ptr du,
                         const Pointer<Potential>& potential);

        void setTemperature(Real value);
        void setDissipationCoefficient(Real value);

    protected:
        void startStep_KG(const FieldState &state, Real t, Real dt) override;


        FieldState &F_KG(const FieldState &stateIn, FieldState &stateOut, Real t) override;


    };


}

#endif //STUDIOSLAB_KG_LANGEVINSOLVER_H
