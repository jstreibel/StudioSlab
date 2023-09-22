//
// Created by joao on 3/09/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H

#include "Models/KleinGordon/RtoR/Output/RtoRMonitor.h"

namespace Modes {

    class Monitor : public RtoR::Monitor {
    public:
        Monitor(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, Real phiMin, Real phiMax, const Str &name,
                bool showEnergyHistoryAsDensities=true);

        void setInputModes(RealVector A, RealVector Ω);
    };

} // Modes

#endif //STUDIOSLAB_MONITOR_H
