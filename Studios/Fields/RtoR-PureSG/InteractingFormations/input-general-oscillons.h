//
// Created by joao on 7/19/19.
//

#ifndef V_SHAPE_INPUT_GENERAL_OSCILLONS_H
#define V_SHAPE_INPUT_GENERAL_OSCILLONS_H

#include "../Builder.h"

namespace Studios::PureSG {

    using namespace Slab;

    class InputGeneralOscillons : public Builder {
        RealParameter v1     = RealParameter{.0, "v1", "Left oscillon's border speed."};
        RealParameter v2     = RealParameter{.0, "v2", "Right oscillon's border speed."};
        RealParameter alpha1 = RealParameter{.0, "a1", "Left oscillon's phase (alpha)."};
        RealParameter alpha2 = RealParameter{.0, "a2", "Right oscillon's phase (alpha)."};
        RealParameter l      = RealParameter{1., "l", "Right oscillon's scale factor (lambda)."};
        RealParameter V      = RealParameter{.0, "u", "Each oscillon's speed."};
        RealParameter xTouchLocation = RealParameter{.0, "xTouch", "Each oscillon's speed."};
        BoolParameter mirror = BoolParameter{false, "mirror,+", "Flag to signal use of negative relative signal between input oscillons field value."};

    public:
        InputGeneralOscillons();
        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;
    };
}


#endif //V_SHAPE_INPUT_GENERAL_OSCILLONS_H
