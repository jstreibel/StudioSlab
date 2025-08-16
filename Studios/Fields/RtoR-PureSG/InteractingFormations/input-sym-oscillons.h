#ifndef SIMGENERALOSCILLON_H
#define SIMGENERALOSCILLON_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"
#include "../Builder.h"

namespace Studios::PureSG {

    using namespace Slab::Core;

    class InputSymmetricOscillon : public Builder {
        RealParameter v               = RealParameter(.0,    FParameterDescription{'v',         "Each oscillon's border speed"});
        RealParameter V               = RealParameter(.0,    FParameterDescription{'u',         "Each oscillon's speed"});
        RealParameter alpha           = RealParameter(.0,    FParameterDescription{"alpha",     "Each oscillon's phase"});
        BoolParameter isAntiSymmetric = BoolParameter(false, FParameterDescription{'a', "asym", "Choose with anti-symmetrical oscillons"});

    public:
        InputSymmetricOscillon();

        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;


    };
}

#endif // SIMGENERALOSCILLON_H
