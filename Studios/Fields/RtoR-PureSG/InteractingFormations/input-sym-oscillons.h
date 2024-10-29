#ifndef SIMGENERALOSCILLON_H
#define SIMGENERALOSCILLON_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBuilder.h"
#include "../Builder.h"

namespace Studios::PureSG {

    using namespace Slab::Core;

    class InputSymmetricOscillon : public Builder {
        RealParameter v               = RealParameter(.0, "v", "Each oscillon's border speed");
        RealParameter V               = RealParameter(.0, "u", "Each oscillon's speed");
        RealParameter alpha           = RealParameter(.0, "alpha", "Each oscillon's phase");
        BoolParameter isAntiSymmetric = BoolParameter(false, "asym,a", "Choose with anti-symmetrical oscillons");

    public:
        InputSymmetricOscillon();

        auto getBoundary() -> Math::Base::BoundaryConditions_ptr override;


    };
}

#endif // SIMGENERALOSCILLON_H
