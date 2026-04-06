//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "KG-RtoREquationState.h"

#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Utils/DeviceConfig.h"
#include "Models/KleinGordon/KG-Solver.h"

namespace Slab::Models::KGRtoR {

    class FSystemGordonGPU : Fields::KleinGordon::Solver<FEquationState> {
    public:
        FSystemGordonGPU(const NumericConfig &params,
                         MyBase::EqBoundaryCondition &du,
                         PotentialFunc &potential);

        FEquationState &dtF(const FEquationState &in, FEquationState &out, DevFloat t, DevFloat dt) override;

    private:
        DeviceVector temp;
    };

    using SystemGordonGPU [[deprecated("Use FSystemGordonGPU")]] = FSystemGordonGPU;

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
