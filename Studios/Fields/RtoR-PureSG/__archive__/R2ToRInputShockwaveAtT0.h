//
// Created by joao on 23/03/2021.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVEATT0_H
#define V_SHAPE_R2TORINPUTSHOCKWAVEATT0_H


//#include <View/Graphic/OutputOpenGL.h>
//#include "Apps/Simulations/BCInterface.h"

namespace Slab::Math::R2toR {

    class R2toRInputShockwaveAt_t0 : public Base::BCInterface {
    private:
        Real theta=0.;
        Real e=0.;
        Real t0=0;
    public:

        void YoureTheChosenOne(CLVariablesMap &vm) override;

        auto getOutputs(bool usingOpenGLBackend) const -> Vector<OutputChannel *> override;

        auto getGeneralDescription() const -> std::string override;

    private:
        //Base::OutputOpenGL *outGL;
        Vector<OutputChannel *> myOutputs;
    };

}

#endif //V_SHAPE_R2TORINPUTSHOCKWAVEATT0_H
