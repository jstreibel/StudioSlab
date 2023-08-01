//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_RTORBUILDER_H
#define STUDIOSLAB_RTORBUILDER_H

#include "Mappings/RtoR/View/Graphics/RtoRMonitor.h"
#include "Phys/Numerics/VoidBuilder.h"

namespace RtoR {
    class RtoRBCInterface : public Base::Simulation::VoidBuilder {
    protected:
        virtual auto buildOpenGLOutput() -> Monitor*;

    public:
        explicit RtoRBCInterface(Str name,
                                 Str generalDescription);

        auto buildOutputManager()   -> OutputManager * override;

        auto newFunctionArbitrary() -> void * override;
        auto newFieldState()        -> void * override;
        auto getEquationSolver()    -> void * override;

        auto getInitialState()      -> void * override;

        auto buildStepper()         -> Method * override;
    };
}

#endif //STUDIOSLAB_RTORBUILDER_H
