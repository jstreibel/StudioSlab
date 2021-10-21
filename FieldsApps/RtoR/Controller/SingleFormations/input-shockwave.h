#ifndef SIMSHOCKWAVE_H
#define SIMSHOCKWAVE_H

#include "FieldsApps/RtoR/Controller/RtoRBCInterface.h"
#include "Core/Device.h"
#include "Apps/Simulations/BCInterface.h"

namespace RtoR {
    class InputShockwave : public RtoRBCInterface {
    private:
        auto _getDeltaTypeAsString() const -> String;

        unsigned int deltaType = 1;
        DoubleParameter a0 = DoubleParameter(1., "a0", "Shockwave discontinuity value.");
        DoubleParameter E = DoubleParameter(1., "E", "Shockwave simulation initial (and total) energy.");

    public:
        InputShockwave();

        auto getBoundary() const -> const void * override;

        auto getOutputs(bool usingOpenGLBackend) const -> OutputCollection;

    };
}

#endif // SIMSHOCKWAVE_H
