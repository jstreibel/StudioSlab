//
// Created by joao on 25/03/25.
//

#ifndef SPI_RECIPE_H
#define SPI_RECIPE_H

#include <Core/Controller/CommandLine/CommandLineInterface.h>
#include <Math/Numerics/NumericalRecipe.h>

#include "SPINumericConfig.h"

namespace Slab::Models::StochasticPathIntegrals {

    /*!
     * Stochastic path-integral recipe
     */
    class SPIRecipe : public Math::Base::NumericalRecipe {
        TPointer<SPINumericConfig> SPI_NumericConfig;
        public:
        explicit SPIRecipe(const TPointer<SPINumericConfig> &numeric_config = New<SPINumericConfig>());

        auto buildOutputSockets() -> Math::Base::OutputSockets override;

        auto buildStepper() -> TPointer<Math::Stepper> override;
    };

} // StochasticPathIntegrals::Models::Slab

#endif //SPI_RECIPE_H
