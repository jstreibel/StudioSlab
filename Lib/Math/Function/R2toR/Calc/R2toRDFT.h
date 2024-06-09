//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_R2TORDFT_H
#define STUDIOSLAB_R2TORDFT_H

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math::R2toR {

    class R2toRDFT {
    public:
        typedef R2toC::NumericFunction_ptr FFTData;

        enum DataPolicy {
            Mangle,
            Keep,
            Auto
        };

        enum Transform {
            Fourier=-1,
            InverseFourier=1
        };

        static auto DFTReal_symmetric(const R2toR::NumericFunction &in) -> FFTData;

        static auto DFTReal   (const R2toR::NumericFunction &in, Transform transform=Fourier,
                               DataPolicy inputPolicy=Auto, DataPolicy outputPolicy=Auto) -> FFTData;
        static auto DFTComplex(const R2toC::NumericFunction &in, Transform transform=Fourier,
                               DataPolicy inputPolicy=Auto, DataPolicy outputPolicy=Auto) -> FFTData;

        static auto DFT_inverse_symmetric(const R2toC::NumericFunction &in) -> FFTData;



    };

} // R2toR

#endif //STUDIOSLAB_R2TORDFT_H
