//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMANYOSCILLONS_H
#define V_SHAPE_INPUTMANYOSCILLONS_H

#include "../ThermalBuilder.h"

namespace Studios::Fields::RtoRThermal {
    class FManyOscillonsBuilder  : public FBuilder {

        IntegerParameter nOscillons = IntegerParameter(100, FParameterDescription{"n_osc", "Number of oscillons distributed."});
    public:
        FManyOscillonsBuilder();
        auto GetBoundary() -> Base::BoundaryConditions_ptr override;

    protected:
        //auto buildOpenGLOutput() -> Monitor * override;

    };

    using ManyOscillonsBuilder [[deprecated("Use FManyOscillonsBuilder")]] = FManyOscillonsBuilder;
}


#endif //V_SHAPE_INPUTMANYOSCILLONS_H
