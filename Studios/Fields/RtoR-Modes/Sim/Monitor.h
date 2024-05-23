//
// Created by joao on 3/09/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Graphics/Graph/ℝ↦ℝ/RtoRMonitor.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;

    class Monitor : public RtoR::Monitor {
    public:
        Monitor(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, Real phiMin, Real phiMax, const Str &name,
                bool showEnergyHistoryAsDensities=true);

        void setInputModes(RealVector A, RealVector Ω);
    };

} // Modes

#endif //STUDIOSLAB_MONITOR_H
