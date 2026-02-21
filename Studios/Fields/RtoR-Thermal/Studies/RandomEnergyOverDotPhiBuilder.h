//
// Created by joao on 7/25/22.
//

#ifndef STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H
#define STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H

#include "../ThermalBuilder.h"

namespace Studios::Fields::RtoRThermal {
    class FRandomEnergyOverDotPhiBuilder : public FBuilder {
        RealParameter E = RealParameter{1. / 24, FParameterDescription{'E', "Total energy over space."}};

    public:
        FRandomEnergyOverDotPhiBuilder();

        auto GetBoundary() -> Base::BoundaryConditions_ptr override;
    };

    using RandomEnergyOverDotPhiBuilder [[deprecated("Use FRandomEnergyOverDotPhiBuilder")]] = FRandomEnergyOverDotPhiBuilder;

}


#endif //STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H
