//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "RtoRMonitor.h"
#include "../KGBuilder.h"

namespace RtoR {
    class KGBuilder : public Fields::KleinGordon::KGBuilder {
        IntegerParameter Potential = IntegerParameter(2, "U,potential", "Potential of wave equation:"
                                                                        "\n\t 0: massless"
                                                                        "\n\t 1: Klein-Gordon"
                                                                        "\n\t 2: signum-Gordon" );
        RealParameter mass  = RealParameter(1.0, "mass",   "Mass of the Klein-Gordon potential, if chosen.");

    protected:
        virtual auto buildOpenGLOutput() -> Monitor*;

    public:
        explicit KGBuilder(Str name,
                           Str generalDescription,
                           bool doRegister=false);

        Function *getPotential() const;

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
