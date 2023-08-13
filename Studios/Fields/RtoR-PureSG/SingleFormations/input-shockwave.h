#ifndef SIMSHOCKWAVE_H
#define SIMSHOCKWAVE_H

#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR {
    class InputShockwave : public KGBuilder {
    private:

        unsigned int deltaType = 1;
        RealParameter a0 = RealParameter(1., "a0", "Shockwave discontinuity value.");
        RealParameter E = RealParameter(1., "E", "Shockwave simulation initial (and total) energy.");

    public:
        InputShockwave();

        auto getBoundary() -> void * override;

    };
}

#endif // SIMSHOCKWAVE_H
