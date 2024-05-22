//
// Created by joao on 21/04/24.
//

#ifndef STUDIOSLAB_DFTINVERSE_H
#define STUDIOSLAB_DFTINVERSE_H

#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"
#include "DiscreteFourierTransform.h"

namespace RtoR {

    class DFTInverse {
    public:
        typedef std::shared_ptr<RtoR::DiscreteFunction> DFTInverseFunction_ptr;
        struct Filter {
            Real kMax = 0.0;
            virtual Complex operator()(const Complex &A, Real k) { return A; };
        };

        struct LowPass : public Filter {
            Real kThreshold;
            explicit LowPass(int kThreshold);
            Complex operator()(const Complex &A, Real k) override;
        };
        struct HighPass : public Filter {
            Real kThreshold;
            explicit HighPass(int kThreshold);
            Complex operator()(const Complex &A, Real k) override;
        };

        static DFTInverseFunction_ptr Compute(const DFTResult &dftResult,
                                              Real xMin,
                                              Real L,
                                              Filter *filter=nullptr);
    };

} // RtoR

#endif //STUDIOSLAB_DFTINVERSE_H
