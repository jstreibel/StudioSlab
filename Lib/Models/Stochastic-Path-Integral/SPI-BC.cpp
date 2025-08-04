//
// Created by joao on 25/03/25.
//

#include "SPI-BC.h"

#include <Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h>

#include "SPI-State.h"

namespace Slab::Models::StochasticPathIntegrals {

    void SPIBC::Apply(Math::Base::EquationState &toFunction, Real t) const {
        auto &f = *dynamic_cast<SPIState&>(toFunction).getPhi();

        if ( t == 0 ) f.Apply(Math::RtoR::NullFunction(), f);
    }

}
