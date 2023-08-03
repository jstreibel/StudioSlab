//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Phys/DifferentialEquations/EquationSolver.h"
#include "Common/DeviceConfig.h"
#include "Models/KleinGordon/KGSolver.h"

namespace RtoR {

    class SystemGordonGPU : Fields::KleinGordon::Solver<EquationState> {
    public:
        SystemGordonGPU(const NumericParams &params,
                        MyBase::EqBoundaryCondition &du,
                        PotentialFunc &potential);

        EquationState &dtF(const EquationState &in, EquationState &out, Real t, Real dt) override;

    private:
        DeviceVector temp;
    };

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
