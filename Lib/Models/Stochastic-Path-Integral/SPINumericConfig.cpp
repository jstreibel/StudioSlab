//
// Created by joao on 26/03/25.
//

#include "SPINumericConfig.h"

#include <sigc++/adaptors/retype.h>

namespace Slab::Models::StochasticPathIntegrals {
    SPINumericConfig::SPINumericConfig() : NumericConfig(false) {
        getInterface()->addParameters({&L, &t, &N, &dT, &nT});
    }

    auto SPINumericConfig::getL() const -> Real { return *L; }

    auto SPINumericConfig::gett() const -> Real { return *t; }

    auto SPINumericConfig::getN() const -> UInt {return *N; }

    auto SPINumericConfig::getdT() const -> Real { return *dT; }

    auto SPINumericConfig::getn() const -> UInt { return *nT; }

    auto SPINumericConfig::to_string() const -> Str {
        return "<StochasticPathIntegrals>";
    }
}
