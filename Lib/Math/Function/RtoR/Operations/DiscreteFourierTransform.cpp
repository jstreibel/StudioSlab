//
// Created by joao on 2/09/23.
//

#include "DiscreteFourierTransform.h"

#include "Math/Constants.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include <fftw3.h>
#include <cstring>

#define RE_PART 0
#define IM_PART 1

namespace Slab::Math::RtoR {

    DFTResult DFT::Compute(const RtoR::NumericFunction &inFunc) {
        if(inFunc.getSpace().dataOnGPU()) throw Exception("DFT of GPU data is not implemented");

        fix N = (int)inFunc.N;
        fix h = inFunc.getSpace().getMetaData().geth(0);
        fix L = N*h;

        IN hostData = &inFunc.getSpace().getHostData()[0];
        auto in = (double *) fftw_malloc(sizeof(double) * N);
        std::memcpy(in, hostData, sizeof(double) * N);

        auto out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
        fftw_plan p = fftw_plan_dft_r2c_1d((int) N, in, out, FFTW_ESTIMATE);
        fftw_execute(p);

        fix Δk = 2 * Constants::pi / L;  // Delta k
        DFTResult result(true);

        fix N⁻¹ = 1./N;
        for( auto n=0; n<N/2+1; ++n) {
            fix A = out[n];
            fix A_Re = A[RE_PART] * N⁻¹;
            fix A_Im = A[IM_PART] * N⁻¹;

            fix ω = Δk * n;

            result.re->addPoint({ω, A_Re});
            result.im->addPoint({ω, A_Im});
        }


        // Cleanup
        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);


        return result;
    }

    DFTResult DFT::Compute(const Function &f, NumberOfModes N, Real xMin, Real xMax) {
        fix L = xMax - xMin;
        fix dx = L / (Real) N;

        auto in = (double *) fftw_malloc(sizeof(double) * N);

        for (auto i = 0; i < N; ++i) {
            auto x = xMin + i*dx;
            in[i] = f(x);
        }

        fix Nₒᵤₜ = N/2 + 1;

        auto out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * Nₒᵤₜ);
        fftw_plan p = fftw_plan_dft_r2c_1d((int) N, in, out, FFTW_ESTIMATE);
        fftw_execute(p);

        fix Δk = 2 * Constants::pi / L;  // Delta k
        DFTResult result(true);

        fix scale = 1./(double)Nₒᵤₜ;
        for( auto n=0; n<Nₒᵤₜ; ++n) {
            fix A = out[n];
            fix A_Re = A[RE_PART] * scale;
            fix A_Im = A[IM_PART] * scale;

            fix ω = Δk * n;

            result.re->addPoint({ω, A_Re});
            result.im->addPoint({ω, A_Im});
        }

        fftw_destroy_plan(p);
        fftw_free(in);
        fftw_free(out);

        return result;
    }

    Pointer<RtoR::NumericFunction> DFT::Magnitudes(const DFTResult &dftResult) {
        auto mags = dftResult.getMagnitudes()->getPoints();

        auto Δk = mags.back().x;
        Core::Log::Debug() << "Computing magnitudes for dft result with Δk=" << Δk << Core::Log::Flush;
        fix n = mags.size();

        auto func = Slab::New<NumericFunction_CPU>(n, 0, Δk);
        auto&space = func->getSpace().getHostData();
        size_t i=0;
        for(auto &pt : mags){
            fix A = pt.y;
            space[i] = A;
            ++i;
        }

        return func;
    }

} // R2toR