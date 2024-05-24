#ifndef SIMSHOCKWAVE_H
#define SIMSHOCKWAVE_H

#include "../Builder.h"

namespace Studios::PureSG {
    class InputShockwave : public Builder {
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
