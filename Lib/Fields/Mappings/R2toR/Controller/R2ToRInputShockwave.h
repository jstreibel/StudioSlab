//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTSHOCKWAVE_H

#include <View/Graphic/OutputOpenGL.h>
#include "Apps/Simulations/BCInterface.h"

namespace R2toR {

    class R2toRInputShockwave : public Base::BCInterface {
    private:
        Real eps=1.;
        Real theta=0.;
        Real E=1.;
        Real e=0.;
        Real t0=0;
    public:
        auto getBoundary() const -> const void * override = 0;

        auto getOutputs(bool usingOpenGLBackend) const -> std::vector<OutputChannel *> override;

        auto getGeneralDescription() const -> std::string override;

    private:
        Base::OutputOpenGL *outGL;
        std::vector<OutputChannel *> myOutputs;
    };

}


#endif //V_SHAPE_R2TORINPUTSHOCKWAVE_H
