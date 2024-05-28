//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMANYOSCILLONS_H
#define V_SHAPE_INPUTMANYOSCILLONS_H

#include "../ThermalBuilder.h"

namespace Studios::Fields::RtoRThermal {
    class ManyOscillonsBuilder  : public Builder {

        IntegerParameter nOscillons = IntegerParameter(100, "n_osc", "Number of oscillons distributed.");
    public:
        ManyOscillonsBuilder();
        auto getBoundary() -> Base::BoundaryConditions_ptr override;

    protected:
        //auto buildOpenGLOutput() -> Monitor * override;

    };
}


#endif //V_SHAPE_INPUTMANYOSCILLONS_H
