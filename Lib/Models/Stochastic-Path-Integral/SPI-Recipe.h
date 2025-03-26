//
// Created by joao on 25/03/25.
//

#ifndef SPI_RECIPE_H
#define SPI_RECIPE_H

#include <Core/Controller/CommandLine/CLInterface.h>
#include <Math/Numerics/NumericalRecipe.h>

namespace Slab::Models::StochasticPathIntegrals {
    /*!
     * Stochastic path-integral recipe
     */
    class SPIRecipe : public Slab::Math::Base::NumericalRecipe {

        Core::RealParameter L = Core::RealParameter(1.0, "L,length", "Space length");
        Core::RealParameter t = Core::RealParameter(1.0, "t,time", "Time interval");
        Core::RealParameter N = Core::RealParameter(1.0, "N,site_count", "Total sites in space "
                                                                         "direction. Time sites are computed "
                                                                         "proportionally from this.");
        Core::RealParameter dT = Core::RealParameter(0.1, "d,dT", "Stochastic time-step size");
        Core::IntegerParameter nT = Core::IntegerParameter(1000, "n,stochastic_time_steps",
            "Stochastic time-steps to be taken until expected end of transient");

        public:
        explicit SPIRecipe(const Pointer<Math::NumericConfig> &numeric_config,
                           const Str &name="Stochastic Path Integrals",
                           const Str &generalDescription="Recipe for generating relevant field configurations in the "
                                                         "(1+1) dimensional, real valued fields quantum path integral "
                                                         "formalism.",
                           bool doRegister=true);



        auto buildOutputSockets() -> Math::Base::OutputSockets override;

        auto buildStepper() -> Pointer<Math::Stepper> override;
    };

} // StochasticPathIntegrals::Models::Slab

#endif //SPI_RECIPE_H
