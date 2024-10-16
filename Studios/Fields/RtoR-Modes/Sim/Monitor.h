//
// Created by joao on 3/09/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"
#include "Models/KleinGordon/KGNumericConfig.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;

    using KGNumericConfig = Models::KGNumericConfig;

    class Monitor : public Slab::Models::KGRtoR::Monitor {
    public:
        Monitor(const Pointer<KGNumericConfig> &params, Slab::Models::KGRtoR::KGEnergy &hamiltonian, Real phiMin, Real phiMax, const Str &name,
                bool showEnergyHistoryAsDensities=true);

        void setInputModes(RealVector A, RealVector k, RealVector Ω);
    };

} // Modes

#endif //STUDIOSLAB_MONITOR_H
