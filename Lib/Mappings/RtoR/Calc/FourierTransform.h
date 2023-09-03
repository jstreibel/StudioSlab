//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_FOURIERTRANSFORM_H
#define STUDIOSLAB_FOURIERTRANSFORM_H

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"

namespace RtoR {

    typedef std::shared_ptr<RtoR::DiscreteFunction> DFPtr;

    struct FTResult {
        DFPtr realPart;
        DFPtr imaginaryPart;

    };

    class FourierTransform {
    public:

        static FTResult Compute(const RtoR::DiscreteFunction &in);
    };

} // R2toR

#endif //STUDIOSLAB_FOURIERTRANSFORM_H
