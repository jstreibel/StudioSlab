//
// Created by joao on 26/03/25.
//

#include "SPINumericConfig.h"

#include <sigc++/adaptors/retype.h>

namespace Slab::Models::StochasticPathIntegrals {
    SPINumericConfig::SPINumericConfig() : FNumericConfig(false) {
        GetInterface()->AddParameters({&L, &t, &N, &dT, &nT});
    }

    auto SPINumericConfig::getL() const -> DevFloat { return *L; }

    auto SPINumericConfig::gett() const -> DevFloat { return *t; }

    auto SPINumericConfig::getN() const -> UInt {return *N; }

    auto SPINumericConfig::getdT() const -> DevFloat { return *dT; }

    auto SPINumericConfig::Get_n() const -> UIntBig { return *nT; }

    auto SPINumericConfig::to_string() const -> Str {
        return "<StochasticPathIntegrals>";
    }
}
