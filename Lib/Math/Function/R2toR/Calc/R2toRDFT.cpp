//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"

#define DONT_FIX_OUTPUT
#include "UnmangleFFTWOutput.h"


namespace R2toR {

    auto R2toRDFT::DFT(const R2toC::DiscreteFunction &in, int sign) -> std::shared_ptr<R2toC::DiscreteFunction> {
        fix M = (int)in.M;
        fix N = (int)in.N;

        // const_cast because fftw does not accept constant data sources.
        auto* data_src_raw /*(N * M)*/ = &in.getData()[0];
        auto* data_src = fftw_alloc_complex(sizeof(fftw_complex) * N * M);
        auto* data_dft = fftw_alloc_complex(sizeof(fftw_complex) * N * M);

        for(int i=0; i<N; ++i) {
            for (int j=0; j < M; ++j) {
                fix &f = data_src_raw[i+j*N];
                auto *z = data_src[i+j*N];

                z[0] = f.real();
                z[1] = f.imag();
            }
        }

        auto plan = fftw_plan_dft_2d(M, N, data_src, data_dft, sign, FFTW_ESTIMATE);

        fftw_execute(plan);

        fix Lx = in.Lx;
        fix Ly = in.Ly;

        fix dk = M_PI / Lx;
        fix dω = M_PI / Ly;
        fix Δk = N * dk;
        fix Δω = M * dω;

        auto out = new R2toC::DiscreteFunction(N, M, -Δk/2, -Δω/2, Δk, Δω);
        auto &data_out = out->getData();
        fix scale = Lx*Ly/(N*M);

        fix halfN = N/2;
        fix halfM = M/2;
        for(int i=0; i<N; ++i) {
            FIX_i_out

            for(int j=0; j<M; ++j) {
                FIX_j_out

                fix k_in = i + j*N;
                fix k_out = i_out + j_out*N;

                fix &z = data_dft[k_in];
                data_out[k_out] = scale * Complex(z[0], z[1]);
            }
        }

        fftw_destroy_plan(plan);
        fftw_free(data_src);
        fftw_free(data_dft);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};    }

    auto R2toRDFT::DFTReal(const DiscreteFunction &in, int sign) -> std::shared_ptr<R2toC::DiscreteFunction> {
        fix M = (int)in.getM();
        fix N = (int)in.getN();

        // const_cast because fftw does not accept constant data sources.
        auto* data_src_raw /*(N * M)*/ = const_cast<Real*>(&in.getSpace().getHostData(false)[0]);
        auto* data_src = fftw_alloc_complex(sizeof(fftw_complex) * N * M);
        auto* data_dft = fftw_alloc_complex(sizeof(fftw_complex) * N * M);

        for(int i=0; i<N; ++i) {
            for (int j=0; j < M; ++j) {
                fix &f = data_src_raw[i+j*N];
                auto *z = data_src[i+j*N];

                z[0] = f;
                z[1] = 0.0;
            }
        }

        auto plan = fftw_plan_dft_2d(M, N, data_src, data_dft, sign, FFTW_ESTIMATE);

        fftw_execute(plan);

        fix Lx = in.getDomain().getLx();
        fix Ly = in.getDomain().getLy();

        fix dk = M_PI / Lx;
        fix dω = M_PI / Ly;
        fix Δk = N * dk;
        fix Δω = M * dω;

        auto out = new R2toC::DiscreteFunction(N, M, -Δk/2, -Δω/2, Δk, Δω);
        auto &data_out = out->getData();
        fix scale = Lx*Ly/(N*M);

        fix halfN = N/2;
        fix halfM = M/2;
        for(int i=0; i<N; ++i) {
            FIX_i_out

            for(int j=0; j<M; ++j) {
                FIX_j_out

                fix k_in = i + j*N;
                fix k_out = i_out + j_out*N;

                fix &z = data_dft[k_in];
                data_out[k_out] = scale * Complex(z[0], z[1]);
            }
        }

        fftw_destroy_plan(plan);
        fftw_free(data_src);
        fftw_free(data_dft);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};
    }

    auto moveData(const fftw_complex *in, ComplexArray &out, UInt n, UInt M, Real scaleFactor) {

        int halfM = (int)M/2;
        for (int i = 0; i < n; ++i) {
            for (auto j = 0; j < M; ++j) {
                FIX_j_out

                fix &z_in = in [i + j*n];
                auto &z_out = out[i + j_out*n];
                z_out = scaleFactor * Complex {z_in[0], z_in[1]};
            }
        }
    }

    auto R2toRDFT::DFTReal_symmetric(const DiscreteFunction &toTransform) -> std::shared_ptr<R2toC::DiscreteFunction> {
        if(toTransform.getN()%2) throw Exception("can't FT real data with odd number of sites in space dimension");

        fix M = toTransform.getM();
        fix N = toTransform.getN();

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