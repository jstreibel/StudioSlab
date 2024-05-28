//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Math/DifferentialEquations/Solver.h"
#include "Utils/DeviceConfig.h"
#include "Models/KleinGordon/KGSolver.h"

namespace Slab::Models::KGRtoR {

    class SystemGordonGPU : Fields::KleinGordon::Solver<EquationState> {
    public:
        SystemGordonGPU(const NumericConfig &params,
                        MyBase::EqBoundaryCondition &du,
                        PotentialFunc &potential);

        EquationState &dtF(const EquationState &in, EquationState &out, Real t, Real dt) override;

    private:
        DeviceVector temp;
    };

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
