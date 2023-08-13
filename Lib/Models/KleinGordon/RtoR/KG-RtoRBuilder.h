//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "RtoRMonitor.h"
#include "../KGBuilder.h"

namespace RtoR {
    class KGBuilder : public Fields::KleinGordon::KGBuilder {
    protected:
        virtual auto buildOpenGLOutput() -> Monitor*;

    public:
        explicit KGBuilder(Str name,
                           Str generalDescription,
                           bool doRegister=false);

        auto buildOutputManager()   -> OutputManager * override;
        auto buildEquationSolver()  -> void * override;
        auto buildStepper()         -> Stepper * override;

        void *getHamiltonian() override;

        auto getInitialState()      -> void * override;

        auto newFunctionArbitrary() -> void * override;
        auto newFieldState()        -> void * override;


    };
}

#endif //STUDIOSLAB_KG_RTORBUILDER_H
