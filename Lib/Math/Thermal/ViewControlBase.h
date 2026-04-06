//
// Created by joao on 22/07/2021.
//

#ifndef ISING_VIEWCONTROLBASE_H
#define ISING_VIEWCONTROLBASE_H


#include "Math/Thermal/InputOutput.h"

namespace Slab::Lost::ThermoOutput {
    class FViewControlBase {
    public:
        virtual bool doOperate(SystemParams &params, OutputData &data) = 0;
    };

    using ViewControlBase [[deprecated("Use FViewControlBase")]] = FViewControlBase;
}

#endif //ISING_VIEWCONTROLBASE_H
