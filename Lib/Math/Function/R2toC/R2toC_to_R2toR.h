//
// Created by joao on 13/05/24.
//

#ifndef STUDIOSLAB_R2TOC_TO_R2TOR_H
#define STUDIOSLAB_R2TOC_TO_R2TOR_H

#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"
#include "R2toCDiscreteFunction.h"

namespace Slab::Math {
    enum R2toC_to_R2toR_Mode {
        RealPart,
        ImaginaryPart,
        Phase,
        Magnitude,
        PowerSpectrum
    };

    R2toR::DiscreteFunction_ptr Convert                 (const R2toC::DiscreteFunction_constptr&, R2toC_to_R2toR_Mode);

    R2toR::DiscreteFunction_ptr ConvertToAbs            (const R2toC::DiscreteFunction_constptr& in,
                                                                 R2toR::DiscreteFunction_ptr out);
    R2toR::DiscreteFunction_ptr ConvertToPowerSpectrum  (const R2toC::DiscreteFunction_constptr& in,
                                                                 R2toR::DiscreteFunction_ptr out);
    R2toR::DiscreteFunction_ptr ConvertToPhase          (const R2toC::DiscreteFunction_constptr& in,
                                                                 R2toR::DiscreteFunction_ptr out);
    R2toR::DiscreteFunction_ptr ConvertToReal           (const R2toC::DiscreteFunction_constptr &in,
                                                                 R2toR::DiscreteFunction_ptr out);
    R2toR::DiscreteFunction_ptr ConvertToImaginary      (const R2toC::DiscreteFunction_constptr &in,
                                                                 R2toR::DiscreteFunction_ptr out);
}

#endif //STUDIOSLAB_R2TOC_TO_R2TOR_H
