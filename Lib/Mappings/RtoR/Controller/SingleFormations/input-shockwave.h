#ifndef SIMSHOCKWAVE_H
#define SIMSHOCKWAVE_H

#include "Mappings/RtoR/Controller/RtoRBCInterface.h"

#include "Phys/Numerics/Output/Plugs/Plug.h"

namespace RtoR {
    class InputShockwave : public RtoRBCInterface {
    private:
        auto _getDeltaTypeAsString() const -> String;

        unsigned int deltaType = 1;
        RealParameter a0 = RealParameter(1., "a0", "Shockwave discontinuity value.");
        RealParameter E = RealParameter(1., "E", "Shockwave simulation initial (and total) energy.");

    public:
        InputShockwave();

        auto getBoundary() const -> const void * override;

        auto getOutputs(bool usingOpenGLBackend) const -> OutputCollection;

    };
}

#endif // SIMSHOCKWAVE_H