//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "RtoRFieldState.h"

#include "Phys/DifferentialEquations/EquationSolver.h"
#include "Common/DeviceConfig.h"
#include "Phys/DifferentialEquations/2nd-Order/GordonSystemT.h"

namespace RtoR {

    class SystemGordonGPU : Phys::Gordon::GordonSolverT<EquationState> {
    public:
        SystemGordonGPU(const NumericParams &params);
        EquationState &dtF(const EquationState &in, EquationState &out, Real t, Real dt) override;

    private:
        DeviceVector temp;
    };

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
