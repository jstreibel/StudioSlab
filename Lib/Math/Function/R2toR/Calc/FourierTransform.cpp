//
// Created by joao on 2/09/23.
//

#include "FourierTransform.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"

#include <fftw3.h>

#define REAL_PART 0

namespace Slab::Math::R2toR {
    Pointer<R2toR::FNumericFunction> FourierTransform::Compute(const R2toR::FNumericFunction &inFunc) {
        if(inFunc.getSpace().dataOnGPU()) throw "DFT of GPU data is not implemented";

        fix N = inFunc.getN();
        fix M = inFunc.getM();
        fix D = inFunc.getDomain();
        fix hx = inFunc.getSpace().getMetaData().geth(0);
        fix hy = inFunc.getSpace().getMetaData().geth(1);
        fix outN = N;
        fix outM = M/2 + 1;
        auto fourierTransformedField =
                DataAlloc<R2toR::NumericFunction_CPU>(Str("ℱ[")+inFunc.get_data_name()+"]",
                                                        N, M, D.xMin, D.yMin, hx, hy);


        IN hostData = &inFunc.getSpace().getHostData()[0];
        auto *in = (double*) fftw_malloc(sizeof(double) * N * M);
        std::memcpy(in, hostData, sizeof(double) * N * M);

        auto *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * outN * outM);

        fftw_plan p = fftw_plan_dft_r2c_2d((int)N, (int)M, in, out, FFTW_ESTIMATE);
        fftw_execute(p);

        // Now, out contains the Fourier coefficients.
        // out[k][l][0] will contain the real part and out[k][l][1] will contain the imaginary part
        // for the Fourier mode corresponding to the wave numbers k and l.
        for (auto k=0; k<outN; ++k) {
            for (auto l = 0; l < outM; l++) {
                fix val = out[k+l*outN];
                fourierTransformedField->At(k, l) = val[REAL_PART];
            }
        }

        // Cleanup
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);

        return fourierTransformedField;
    }
} // R2toR