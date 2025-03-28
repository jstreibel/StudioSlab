//
// Created by joao on 13/05/24.
//

#ifndef STUDIOSLAB_R2TOC_TO_R2TOR_H
#define STUDIOSLAB_R2TOC_TO_R2TOR_H

#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "R2toCNumericFunction.h"

namespace Slab::Math {
    enum R2toC_to_R2toR_Mode {
        RealPart,
        ImaginaryPart,
        Phase,
        Magnitude,
        PowerSpectrum
    };

    R2toR::NumericFunction_ptr Convert                 (const R2toC::NumericFunction_constptr&, R2toC_to_R2toR_Mode);

    R2toR::NumericFunction_ptr ConvertToAbs            (const R2toC::NumericFunction_constptr& in,
                                                                 R2toR::NumericFunction_ptr out);
    R2toR::NumericFunction_ptr ConvertToPowerSpectrum  (const R2toC::NumericFunction_constptr& in,
                                                                 R2toR::NumericFunction_ptr out);
    R2toR::NumericFunction_ptr ConvertToPhase          (const R2toC::NumericFunction_constptr& in,
                                                                 R2toR::NumericFunction_ptr out);
    R2toR::NumericFunction_ptr ConvertToReal           (const R2toC::NumericFunction_constptr &in,
                                                                 R2toR::NumericFunction_ptr out);
    R2toR::NumericFunction_ptr ConvertToImaginary      (const R2toC::NumericFunction_constptr &in,
                                                                 R2toR::NumericFunction_ptr out);
}

#endif //STUDIOSLAB_R2TOC_TO_R2TOR_H
