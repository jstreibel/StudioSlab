//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "Graphics/Graph/ℝ↦ℝ/RtoRMonitor.h"
#include "../KGBuilder.h"

namespace RtoR {
    class KGBuilder : public Fields::KleinGordon::KGBuilder {
        IntegerParameter Potential = IntegerParameter(2, "V,potential", "Potential of wave equation:"
                                                                        "\n\t 0: massless"
                                                                        "\n\t 1: Klein-Gordon"
                                                                        "\n\t 2: signum-Gordon" );
        RealParameter massSqr  = RealParameter(1.0, "M,massSqr",   "Squared mass of the Klein-Gordon potential (on-shell ω²-k²-m²=0), if chosen.");

        bool periodicBC = false;          // Gambiarris

    protected:
        virtual auto buildOpenGLOutput() -> Monitor*;

        void setLaplacianPeriodicBC();    // Gambiarris
        void setLaplacianFixedBC();       // Gambiarris
        bool usesPeriodicBC() const;      // Gambiarris

    public:
        auto suggestFileName() const -> Str override;

    public:
        explicit KGBuilder(const Str& name,
                           Str generalDescription,
                           bool doRegister=false);

        Function *getPotential() const;
        virtual Function::Ptr getNonHomogenous();

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
