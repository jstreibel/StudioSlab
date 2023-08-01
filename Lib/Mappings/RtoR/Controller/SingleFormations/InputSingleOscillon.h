//
// Created by joao on 27/09/2021.
//

#ifndef FIELDS_INPUTSINGLEOSCILLON_H
#define FIELDS_INPUTSINGLEOSCILLON_H


#include "Mappings/RtoR/Controller/RtoRBCInterface.h"
#include "../RtoRBuilder.h"


namespace RtoR {
    class InputSingleOscillon : public RtoRBCInterface {
        RealParameter::Ptr v      = RealParameter::New(.0, "v", "Left oscillon's border speed.");
        RealParameter::Ptr V      = RealParameter::New(.0, "V", "Each oscillon's speed.");
        RealParameter::Ptr alpha  = RealParameter::New(.0, "alpha,a", "Left oscillon's phase (alpha).");
        RealParameter::Ptr lambda = RealParameter::New(1., "lambda,l", "Right oscillon's scale factor (lambda).");
        BoolParameter::Ptr mirror = BoolParameter::New(false, "mirror",
                                             "Flag to signal use of negative relative signal between input oscillons field value.");

    public:
        InputSingleOscillon();

        auto getBoundary() -> void * override;
    };
}

#endif //FIELDS_INPUTSINGLEOSCILLON_H
