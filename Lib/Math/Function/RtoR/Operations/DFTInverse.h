//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_DFTINVERSE_H
#define STUDIOSLAB_DFTINVERSE_H

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "DiscreteFourierTransform.h"

namespace Slab::Math::RtoR {

    class FDFTInverse {
    public:
        using FDFTInverseFunction = RtoR::NumericFunction;
        struct FFilter {
            DevFloat kMax = 0.0;
            virtual Complex operator()(const Complex &A, DevFloat k) { return A; };
        };

        struct FLowPass : public FFilter {
            DevFloat kThreshold;
            explicit FLowPass(int kThreshold);
            Complex operator()(const Complex &A, DevFloat k) override;
        };
        struct FHighPass : public FFilter {
            DevFloat kThreshold;
            explicit FHighPass(int kThreshold);
            Complex operator()(const Complex &A, DevFloat k) override;
        };

        static TPointer<FDFTInverseFunction> Compute(const FDFTResult &dftResult,
                                                     DevFloat xMin,
                                                     DevFloat L,
                                                     FFilter *filter = nullptr);

        using DFTInverseFunction [[deprecated("Use FDFTInverseFunction")]] = FDFTInverseFunction;
        using Filter [[deprecated("Use FFilter")]] = FFilter;
        using LowPass [[deprecated("Use FLowPass")]] = FLowPass;
        using HighPass [[deprecated("Use FHighPass")]] = FHighPass;
    };

    using DFTInverse [[deprecated("Use FDFTInverse")]] = FDFTInverse;

} // RtoR

#endif //STUDIOSLAB_DFTINVERSE_H
