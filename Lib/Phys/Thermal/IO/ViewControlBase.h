//
// Created by joao on 22/07/2021.
//

#ifndef ISING_VIEWCONTROLBASE_H
#define ISING_VIEWCONTROLBASE_H


#include "Phys/Thermal/IO/Tools/InputOutput.h"

namespace ThermoOutput {
    class ViewControlBase {
    public:
        virtual bool doOperate(SystemParams &params, OutputData &data) = 0;
    };
}

#endif //ISING_VIEWCONTROLBASE_H
