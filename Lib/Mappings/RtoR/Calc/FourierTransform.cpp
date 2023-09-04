//
// Created by joao on 2/09/23.
//

#include "FourierTransform.h"

#include "Mappings/RtoR/Model/RtoRDiscreteFunctionCPU.h"
#include "Math/Constants.h"

#include <fftw3.h>
#include <cstring>

#define REAL_PART 0
#define IMAGINARY_PART 0

namespace RtoR {
    FTResult FourierTransform::Compute(const RtoR::DiscreteFunction &inFunc) {
        if(inFunc.getSpace().dataOnGPU()) throw "FourierTransform of GPU data is not implemented";

        fix N = (int)inFunc.N;
        fix h = inFunc.getSpace().geth();
        fix L = N*h;


        IN hostData = &inFunc.getSpace().getHostData()[0];
        auto in = (double *) fftw_malloc(sizeof(double) * N);
        std::memcpy(in, hostData, sizeof(double) * N);

        auto out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
        fftw_plan p = fftw_plan_dft_r2c_1d((int) N, in, out, FFTW_ESTIMATE);
        fftw_execute(p);

        fix Δk = 2 * Constants::pi / L;  // Delta k
        auto realPartField = new RtoR::FunctionArbitraryCPU(N/2+1, 0.0, Δk*(N/2.-1));
        auto imPartField = new RtoR::FunctionArbitraryCPU(N/2+1, 0.0, Δk*(N/2.-1));
        auto A_Re = &realPartField->getSpace().getHostData()[0];
        auto A_Im = &realPartField->getSpace().getHostData()[0];
        fix N⁻¹ = 1./N;
        for( auto i=0; i<N/2+1; ++i) {
            fix k = out[i];

            A_Re[i] = k[REAL_PART] * N⁻¹;
            A_Im[i] = k[IMAGINARY_PART] * N⁻¹;
        }


        // Cleanup
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);


        return {DFPtr(realPartField), DFPtr(imPartField)};
    }
} // R2toR