//
// Created by joao on 12/08/23.
//

#ifndef STUDIOSLAB_LANGEVINKGSOLVER_H
#define STUDIOSLAB_LANGEVINKGSOLVER_H

#include "Models/KleinGordon/KGSolver.h"
#include "Models/KleinGordon/KGState.h"

#include "Models/KleinGordon/RtoR/KG-RtoRSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/Numerics/NumericalRecipe.h"

#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"


#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {

    class LangevinKGSolver : public KGRtoRSolver {
        RtoR::DiscreteFunction_ptr langevinImpulses = nullptr;
        RtoR::DiscreteFunction_ptr scaledImpulses   = nullptr;

        Real T=.0;
        Real γ=.0;

        void ComputeImpulses();
    public:
        LangevinKGSolver(const NumericConfig &params,
                         Base::BoundaryConditions_ptr du,
                         Pointer<Potential> potential);

        void setTemperature(Real value);
        void setDissipationCoefficient(Real value);

    protected:
        void startStep_KG(const EquationState &state, Real t, Real dt) override;

        EquationState &dtF_KG(const EquationState &stateIn, EquationState &stateOut, Real t, Real dt) override;


    };


}

#endif //STUDIOSLAB_LANGEVINKGSOLVER_H
