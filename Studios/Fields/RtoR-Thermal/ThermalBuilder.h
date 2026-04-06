//
// Created by joao on 8/4/23.
//

#ifndef STUDIOSLAB_THERMALBUILDER_H
#define STUDIOSLAB_THERMALBUILDER_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Slab::Models;

    class FBuilder : public KGRtoR::FKGRtoR_Recipe {
        RealParameter temperature    = RealParameter(.0, FParameterDescription{'T', "temperature", "The Langevin temperature reservoir's temperature"});
        RealParameter dissipation    = RealParameter(.0, FParameterDescription{'k', "dissipation_coefficient", "The dynamics dissipation coefficient"});
        RealParameter transientGuess = RealParameter(.0, FParameterDescription{'i', "transient_guess", "User guess for transient value"});

    public:
        FBuilder(const Str &name, const Str &generalDescription, bool doRegister=false);

        auto buildSolver() -> TPointer<Base::LinearStepSolver> override;

        auto SuggestFileName() const -> Str override;

    };

    using Builder [[deprecated("Use FBuilder")]] = FBuilder;
}


#endif //STUDIOSLAB_THERMALBUILDER_H
