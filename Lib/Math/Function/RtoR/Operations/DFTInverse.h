//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_DFTINVERSE_H
#define STUDIOSLAB_DFTINVERSE_H

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"
#include "DiscreteFourierTransform.h"

namespace Slab::Math::RtoR {

    class DFTInverse {
    public:
        using DFTInverseFunction = RtoR::NumericFunction;
        struct Filter {
            DevFloat kMax = 0.0;
            virtual Complex operator()(const Complex &A, DevFloat k) { return A; };
        };

        struct LowPass : public Filter {
            DevFloat kThreshold;
            explicit LowPass(int kThreshold);
            Complex operator()(const Complex &A, DevFloat k) override;
        };
        struct HighPass : public Filter {
            DevFloat kThreshold;
            explicit HighPass(int kThreshold);
            Complex operator()(const Complex &A, DevFloat k) override;
        };

        static Pointer<DFTInverseFunction> Compute(const DFTResult &dftResult,
                                              DevFloat xMin,
                                              DevFloat L,
                                              Filter *filter=nullptr);
    };

} // RtoR

#endif //STUDIOSLAB_DFTINVERSE_H
