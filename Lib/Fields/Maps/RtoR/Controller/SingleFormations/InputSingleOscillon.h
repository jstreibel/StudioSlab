//
// Created by joao on 27/09/2021.
//

#ifndef FIELDS_INPUTSINGLEOSCILLON_H
#define FIELDS_INPUTSINGLEOSCILLON_H


#include "../RtoRBCInterface.h"


namespace RtoR {
    class InputSingleOscillon : public RtoRBCInterface {
        DoubleParameter v = DoubleParameter{.0, "v", "Left oscillon's border speed."};
        DoubleParameter V = DoubleParameter{.0, "V", "Each oscillon's speed."};
        DoubleParameter alpha = DoubleParameter{.0, "alpha,a", "Left oscillon's phase (alpha)."};
        DoubleParameter lambda = DoubleParameter{1., "lambda,l", "Right oscillon's scale factor (lambda)."};
        BoolParameter mirror = BoolParameter{false, "mirror",
                                             "Flag to signal use of negative relative signal between input oscillons field value."};

    public:
        InputSingleOscillon();

        auto getBoundary() const -> const void * override;
    };
}

#endif //FIELDS_INPUTSINGLEOSCILLON_H
