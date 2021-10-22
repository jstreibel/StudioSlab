//
// Created by joao on 7/19/19.
//

#ifndef V_SHAPE_INPUT_GENERAL_OSCILLONS_H
#define V_SHAPE_INPUT_GENERAL_OSCILLONS_H

#include "Lib/Fields/Maps/RtoR/Controller/RtoRBCInterface.h"

namespace RtoR {

    class InputGeneralOscillons : public RtoRBCInterface {
        DoubleParameter v1     = DoubleParameter{.0, "v1", "Left oscillon's border speed."};
        DoubleParameter v2     = DoubleParameter{.0, "v2", "Right oscillon's border speed."};
        DoubleParameter alpha1 = DoubleParameter{.0, "a1", "Left oscillon's phase (alpha)."};
        DoubleParameter alpha2 = DoubleParameter{.0, "a2", "Right oscillon's phase (alpha)."};
        DoubleParameter l      = DoubleParameter{1., "l", "Right oscillon's scale factor (lambda)."};
        DoubleParameter V      = DoubleParameter{.0, "V", "Each oscillon's speed."};
        DoubleParameter xTouchLocation = DoubleParameter{.0, "V", "Each oscillon's speed."};
        BoolParameter mirror = BoolParameter{false, "mirror,+", "Flag to signal use of negative relative signal between input oscillons field value."};

    public:
        InputGeneralOscillons();
        auto getBoundary() const -> const void * override;
    };
}


#endif //V_SHAPE_INPUT_GENERAL_OSCILLONS_H
