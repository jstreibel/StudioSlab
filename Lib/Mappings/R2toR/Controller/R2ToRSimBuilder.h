//
// Created by joao on 7/19/23.
//

#ifndef STUDIOSLAB_R2TORSIMBUILDER_H
#define STUDIOSLAB_R2TORSIMBUILDER_H

#include "Phys/Numerics/Builder.h"
#include "Mappings/R2toR/View/R2toROutputOpenGLGeneric.h"

namespace R2toR::Simulation {
    class Builder : public Base::Simulation::Builder {
        Str name = "";

    protected:
        virtual auto buildOpenGLOutput() -> R2toR::OutputOpenGL*;

    public:
        Builder(Str name, Str description);

        auto buildOutputManager()       -> OutputManager * override;

        auto newFunctionArbitrary()     -> void * override;
        auto newFieldState()            -> void * override;
        auto getSystemSolver()          -> void * override;
    };
}


#endif //STUDIOSLAB_R2TORSIMBUILDER_H
