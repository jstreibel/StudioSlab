//
// Created by joao on 8/4/23.
//

#ifndef STUDIOSLAB_THERMALBUILDER_H
#define STUDIOSLAB_THERMALBUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab::Core;
    using namespace Slab::Math;

class Builder : public Slab::Models::KGRtoR::KGBuilder {
        RealParameter temperature    = RealParameter(.0, "T,temperature", "The Langevin temperature reservoir's temperature");
        RealParameter dissipation    = RealParameter(.0, "k,dissipation_coefficient", "The dynamics dissipation coefficient");
        RealParameter transientGuess = RealParameter(.0, "i,transient_guess", "User guess for transient value");

    public:
        Builder(const Str &name, const Str &generalDescription, bool doRegister=false);

        auto buildEquationSolver() -> void * override;

        auto suggestFileName() const -> Str override;

    };
}


#endif //STUDIOSLAB_THERMALBUILDER_H
