//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"

namespace R2toR {

    auto moveData(const fftw_complex *in, ComplexArray &out, UInt n, UInt M, Real scaleFactor) {

        int halfM = (int)M/2;
        for (int i = 0; i < n; ++i) {
            for (auto j = 0; j < M; ++j) {
                fix σ = j<=halfM ? 1 : -1;
                fix j_out = j-1 + σ*halfM;

                fix &z_in = in [i + j*n];
                auto &z_out = out[i + j_out*n];
                z_out = scaleFactor * Complex {z_in[0], z_in[1]};
            }
        }
    }

    auto R2toRDFT::DFT_symmetric(const DiscreteFunction &toTransform) -> std::shared_ptr<R2toC::DiscreteFunction> {
        if(toTransform.getN()%2) throw Exception("can't FT real data with odd number of sites in space dimension");

        fix N = toTransform.getN();
        fix M = toTransform.getM();

        fix n = N/2+1;

        // const_cast because fftw does not accept constant data sources.
        auto* data_src/*(N * M)*/ = const_cast<Real*>(&toTransform.getSpace().getHostData(false)[0]);
        auto* data_dft = fftw_alloc_complex(sizeof(fftw_complex) * n * M);

        auto plan = fftw_plan_dft_r2c_2d((int)M, (int)N, data_src, data_dft, FFTW_ESTIMATE);

        fftw_execute(plan);

        fix Lx = toTransform.getDomain().getLx();
        fix Ly = toTransform.getDomain().getLy();

        // fix n = N;
        // fix m = M/2+1;
        fix dk = 2 * M_PI / Lx;
        fix dω = 2 * M_PI / Ly;
        fix Δk = (Real)N/2 * dk; // N/2 not N/2+1
        fix Δω = M * dω;

        auto out = new R2toC::DiscreteFunction(n, M, 0, -Δω/2, Δk+dk, Δω+ dω);
        auto &data_out = out->getData();
        fix scale = 1e0/(N*M);

        moveData(data_dft, data_out, n, M, scale);

        fftw_destroy_plan(plan);
        fftw_free(data_dft);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};
    }


} // R2toR