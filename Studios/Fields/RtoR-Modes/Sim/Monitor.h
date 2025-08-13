//
// Created by joao on 3/09/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"
#include "Models/KleinGordon/KG-NumericConfig.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;

    using KGNumericConfig = Models::FKGNumericConfig;

    class Monitor : public Slab::Models::KGRtoR::Monitor {
    public:
        Monitor(const TPointer<KGNumericConfig> &params, Slab::Models::KGRtoR::KGEnergy &hamiltonian, const Str &name);

        void setInputModes(FRealVector A, const FRealVector& k, FRealVector Ω);
    };

} // Modes

#endif //STUDIOSLAB_MONITOR_H
