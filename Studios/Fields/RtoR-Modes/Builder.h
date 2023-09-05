//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_BUILDER_H
#define STUDIOSLAB_BUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace Modes {

    class Builder : public RtoR::KGBuilder {
        RealParameter A     = RealParameter(1.0, "A",       "Amplitude of input sine wave");
        RealParameter omega = RealParameter(1.0, "w,omega", "Angular frequency of input sine wave");

    protected:
        auto buildOpenGLOutput() -> RtoR::Monitor * override;

    public:
        explicit Builder(bool doRegister=true);

        auto getBoundary() -> void * override;

        auto notifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_BUILDER_H
