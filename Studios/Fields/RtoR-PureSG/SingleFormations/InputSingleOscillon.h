//
// Created by joao on 27/09/2021.
//

#ifndef FIELDS_INPUTSINGLEOSCILLON_H
#define FIELDS_INPUTSINGLEOSCILLON_H


#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"


namespace RtoR {
    class InputSingleOscillon : public KGBuilder {
        RealParameter v      = RealParameter(.0, "v", "Left oscillon's border speed.");
        RealParameter V      = RealParameter(.0, "V", "Each oscillon's speed.");
        RealParameter alpha  = RealParameter(.0, "alpha,a", "Left oscillon's phase (alpha).");
        RealParameter lambda = RealParameter(1., "lambda,l", "Right oscillon's scale factor (lambda).");
        BoolParameter mirror = BoolParameter(false, "mirror",
                                             "Flag to signal use of negative relative signal between input oscillons field value.");

    public:
        InputSingleOscillon();

        auto getBoundary() -> void * override;
    };
}

#endif //FIELDS_INPUTSINGLEOSCILLON_H
