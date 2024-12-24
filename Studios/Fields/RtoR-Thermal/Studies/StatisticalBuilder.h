//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTSTATISTICAL_H
#define V_SHAPE_INPUTSTATISTICAL_H

#include "../ThermalBuilder.h"

namespace Studios::Fields::RtoRThermal {

    class StatisticalBuilder  : public Builder {
        RealParameter E = RealParameter{1., "E,E0", "Initial distributed energy"};
        IntegerParameter n = IntegerParameter{100, "n,n_osc", "Number of initial oscillons"};

    public:
        StatisticalBuilder();

        auto getBoundary() -> Base::BoundaryConditions_ptr override;
        static auto getDetailedDescription() -> Str;
    };
}


#endif //V_SHAPE_INPUTSTATISTICAL_H
