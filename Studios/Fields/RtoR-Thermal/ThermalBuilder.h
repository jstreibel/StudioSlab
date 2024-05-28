//
// Created by joao on 8/4/23.
//

#ifndef STUDIOSLAB_THERMALBUILDER_H
#define STUDIOSLAB_THERMALBUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Slab::Models;

    class Builder : public KGRtoR::KGRtoRBuilder {
        RealParameter temperature    = RealParameter(.0, "T,temperature", "The Langevin temperature reservoir's temperature");
        RealParameter dissipation    = RealParameter(.0, "k,dissipation_coefficient", "The dynamics dissipation coefficient");
        RealParameter transientGuess = RealParameter(.0, "i,transient_guess", "User guess for transient value");

    public:
        Builder(const Str &name, const Str &generalDescription, bool doRegister=false);

        auto buildEquationSolver() -> Base::Solver_ptr override;

        auto suggestFileName() const -> Str override;

    };
}


#endif //STUDIOSLAB_THERMALBUILDER_H
