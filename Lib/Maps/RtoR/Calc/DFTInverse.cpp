//
// Created by joao on 21/04/24.
//

#include "DFTInverse.h"
#include "Maps/RtoR/Model/RtoRDiscreteFunctionCPU.h"

#include <fftw3.h>

#define INVERSE_FOURIER_TRANSFORM FFTW_BACKWARD

namespace RtoR {
    DFTInverse::DFTInverseFunction_ptr DFTInverse::Compute(const DFTResult &dftResult,
                                                           Real xMin,
                                                           Real L,
                                                           Hint hint)
    {
        if(hint != OriginalFunctionIsRealValued)
            throw NotImplementedException(__PRETTY_FUNCTION__ + Str(" for complex-valued original functions."));

        int n = (int) dftResult.modeCount(); // Number of data points, n=N/2+1 ==> N=(n-1)*2
        int N = 2 * (n - 1);

        fftw_complex *in, *out;
        fftw_plan p;

        // Allocate memory for input and output arrays
        in =  (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

        auto &re = dftResult.re->getPoints();
        auto &im = dftResult.im->getPoints();

        // Populate N-n=n-2 modes of input array with complex conjugates
        // Note: N/2==n-1
        // (because original function is hinted to be Real-valued).
        for (int k=0; k<=N/2; ++k){
            in[k][0] = re[k].y; // Real part
            in[k][1] = im[k].y; // Conjugate imaginary part
        }

        // Fill redundant part
        for (int k=1; k<N/2; ++k){
            in[N-k][0] = +re[k].y; // Real part
            in[N-k][1] = -im[k].y; // Imaginary part
        }

        // Create a plan for inverse Fourier transform
        p = fftw_plan_dft_1d(N, in, out, INVERSE_FOURIER_TRANSFORM, FFTW_ESTIMATE);

        // Execute the plan for inverse transform
        fftw_execute(p);

        // Move data to function
        DFTInverseFunction_ptr func(new DiscreteFunction_CPU(N, xMin, xMin+L));
        auto &vals = func->getSpace().getHostData(true);
        // fix N_inv = 1./N;
        for(int i=0; i<N; ++i)
            vals[i] = out[i][0]; // * N_inv;

        // Free allocated memory and destroy plans
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);

        // Return :)
        return func;
    }
} // RtoR