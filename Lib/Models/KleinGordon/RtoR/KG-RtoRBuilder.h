//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "RtoRMonitor.h"
#include "../KGBuilder.h"

namespace RtoR {
    class Builder : public Fields::KleinGordon::KGBuilder {
    protected:
        virtual auto buildOpenGLOutput() -> Monitor*;

    public:
        explicit Builder(Str name,
                         Str generalDescription);

        auto buildOutputManager()   -> OutputManager * override;
        auto buildEquationSolver()  -> void * override;
        auto buildStepper()         -> Method * override;

        void *getHamiltonian() override;

        auto getInitialState()      -> void * override;

        auto newFunctionArbitrary() -> void * override;
        auto newFieldState()        -> void * override;


    };
}

#endif //STUDIOSLAB_KG_RTORBUILDER_H
