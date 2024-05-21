//
// Created by joao on 10/05/24.
//

#ifndef STUDIOSLAB_R2TORDFT_H
#define STUDIOSLAB_R2TORDFT_H

#include "Math/Function/R2toC/R2toCDiscreteFunction.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

namespace R2toR {

    class R2toRDFT {
    public:
        typedef R2toC::DiscreteFunction_ptr FFTData;

        enum DataPolicy {
            Mangle,
            Keep,
            Auto
        };

        enum Transform {
            Fourier=-1,
            InverseFourier=1
        };

        static auto DFTReal_symmetric(const R2toR::DiscreteFunction &in) -> FFTData;

        static auto DFTReal   (const R2toR::DiscreteFunction &in, Transform transform=Fourier,
                               DataPolicy inputPolicy=Auto, DataPolicy outputPolicy=Auto) -> FFTData;
        static auto DFTComplex(const R2toC::DiscreteFunction &in, Transform transform=Fourier,
                               DataPolicy inputPolicy=Auto, DataPolicy outputPolicy=Auto) -> FFTData;

        static auto DFT_inverse_symmetric(const R2toC::DiscreteFunction &in) -> FFTData;



    };

} // R2toR

#endif //STUDIOSLAB_R2TORDFT_H
