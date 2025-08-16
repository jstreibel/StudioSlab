//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMACHINEGUN_H
#define V_SHAPE_INPUTMACHINEGUN_H

#include "../ThermalBuilder.h"


namespace Studios::Fields::RtoRThermal {
    class MachineGunBuilder  : public Builder {
        RealParameter v1 =   RealParameter{0.5,   FParameterDescription{"v1", "Left oscillon's border speed."}};
        IntegerParameter n = IntegerParameter{10, FParameterDescription{'n', "Number of shooting tiny oscillons."}};

    public:
        MachineGunBuilder();
        auto GetBoundary() -> Base::BoundaryConditions_ptr override;
    };
}


#endif //V_SHAPE_INPUTMACHINEGUN_H
