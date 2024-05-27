//
// Created by joao on 12/08/23.
//

#ifndef STUDIOSLAB_LANGEVINKGSOLVER_H
#define STUDIOSLAB_LANGEVINKGSOLVER_H

#include "Models/KleinGordon/KGSolver.h"
#include "Models/KleinGordon/KGState.h"

#include "Models/KleinGordon/RtoR/KG-RtoRSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/Numerics/VoidBuilder.h"

#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"


#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {

    class LangevinKGSolver : public KGSolver {
        RtoR::DiscreteFunction *langevinImpulses = nullptr;
        RtoR::DiscreteFunction *scaledImpulses   = nullptr;

        Real T=.0;
        Real γ=.0;

    public:
        explicit LangevinKGSolver(const NumericConfig &numericParams, KGSolver::MyBase::EqBoundaryCondition_ptr boundary)
        : KGSolver(numericParams, boundary, *(new RtoR::AbsFunction())) { }

        void startStep(const EqState &stateIn, Real t, Real dt) override;

        void ComputeImpulses();

        void setTemperature(Real value);
        void setDissipationCoefficient(Real value);

        EqState &
        dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override;

    };


}

#endif //STUDIOSLAB_LANGEVINKGSOLVER_H
