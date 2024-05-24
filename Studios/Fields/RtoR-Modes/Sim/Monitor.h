//
// Created by joao on 3/09/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;

    class Monitor : public Slab::Models::KGRtoR::Monitor {
    public:
        Monitor(const NumericConfig &params, Slab::Models::KGRtoR::KGEnergy &hamiltonian, Real phiMin, Real phiMax, const Str &name,
                bool showEnergyHistoryAsDensities=true);

        void setInputModes(RealVector A, RealVector Ω);
    };

} // Modes

#endif //STUDIOSLAB_MONITOR_H
