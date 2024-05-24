//
// Created by joao on 7/25/22.
//

#ifndef STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H
#define STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H

#include "../ThermalBuilder.h"

namespace Studios::Fields::RtoRThermal {
    class RandomEnergyOverDotPhiBuilder : public Builder {
        RealParameter E = RealParameter{1. / 24, "E", "Total energy over space."};

    public:
        RandomEnergyOverDotPhiBuilder();

        auto getBoundary() -> void * override;
    };

}


#endif //STUDIOSLAB_RANDOMENERGYOVERDOTPHIBUILDER_H
