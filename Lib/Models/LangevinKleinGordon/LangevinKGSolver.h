//
// Created by joao on 12/08/23.
//

#ifndef STUDIOSLAB_LANGEVINKGSOLVER_H
#define STUDIOSLAB_LANGEVINKGSOLVER_H

#include "Models/KleinGordon/KGSolver.h"
#include "Models/KleinGordon/KGState.h"

#include "Models/KleinGordon/RtoR/KG-RtoRSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Phys/Numerics/VoidBuilder.h"

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/AbsFunction.h"


#define FType(a) typename FieldState::a

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace RtoR {

    class LangevinKGSolver : public RtoR::KGSolver {
        DiscreteFunction *langevinImpulses = nullptr;
        DiscreteFunction *scaledImpulses   = nullptr;

        Real T=.0;
        Real k=.0;
    public:
        explicit LangevinKGSolver(const NumericParams &numericParams, RtoR::KGSolver::MyBase::EqBoundaryCondition &boundary)
        : RtoR::KGSolver(numericParams, boundary, *(new RtoR::AbsFunction())) { }

        void startStep(Real t, Real dt) override;

        void ComputeImpulses();

        void setTemperature(Real value);
        void setDissipationCoefficient(Real value);

        EqState &
        dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override;

    };


}

#endif //STUDIOSLAB_LANGEVINKGSOLVER_H
