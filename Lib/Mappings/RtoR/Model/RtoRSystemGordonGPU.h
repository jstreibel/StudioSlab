//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "RtoRFieldState.h"

#include "Phys/DifferentialEquations/DifferentialEquation.h"
#include "Common/DeviceConfig.h"

namespace RtoR {

    class SystemGordonGPU : Base::DifferentialEquation<EquationState> {
    public:
        SystemGordonGPU(PosInt N);
        EquationState &dtF(const EquationState &in, EquationState &out, Real t, Real dt) override;

    private:
        DeviceVector temp;
    };

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
