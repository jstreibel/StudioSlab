//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_DFTINVERSE_H
#define STUDIOSLAB_DFTINVERSE_H

#include "Maps/RtoR/Model/RtoRDiscreteFunction.h"
#include "DiscreteFourierTransform.h"

namespace RtoR {

    class DFTInverse {
    public:
        typedef std::shared_ptr<RtoR::DiscreteFunction> DFTInverseFunction_ptr;
        enum Hint {
            None=0,
            OriginalFunctionIsRealValued
        };

        static DFTInverseFunction_ptr Compute(const DFTResult &dftResult,
                                              Real xMin,
                                              Real L,
                                              Hint hint=None);

    };

} // RtoR

#endif //STUDIOSLAB_DFTINVERSE_H
