//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SIMULATIONCONFIG_H
#define STUDIOSLAB_SIMULATIONCONFIG_H

#include "DeviceConfig.h"
#include "NumericConfig.h"

namespace Slab::Math {

    struct SimulationConfig {
        explicit SimulationConfig(bool doRegister = true);

        NumericConfig numericConfig;
        DeviceConfig dev;
    };
}


#endif //STUDIOSLAB_SIMULATIONCONFIG_H
