//
// Created by joao on 27/09/2021.
//

#ifndef FIELDS_INPUTSINGLEOSCILLON_H
#define FIELDS_INPUTSINGLEOSCILLON_H

#include "../Builder.h"

namespace Studios::PureSG {

    using namespace Slab::Core;

    class InputSingleOscillon : public Builder {
        RealParameter v      = RealParameter(.0,    FParameterDescription{"v",           "Oscillon's border speed."});
        RealParameter V      = RealParameter(.0,    FParameterDescription{"u",           "Oscillon's speed."});
        RealParameter alpha  = RealParameter(.0,    FParameterDescription{'a', "alpha",  "Oscillon's initial phase (alpha)."});
        RealParameter lambda = RealParameter(1.,    FParameterDescription{'l', "lambda", "Oscillon's scale factor (lambda)."});
        BoolParameter mirror = BoolParameter(false, FParameterDescription{"mirror",      "Flag to signal use of negative relative signal between input oscillons field value."});

    public:
        InputSingleOscillon();

        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;


    };
}

#endif //FIELDS_INPUTSINGLEOSCILLON_H
