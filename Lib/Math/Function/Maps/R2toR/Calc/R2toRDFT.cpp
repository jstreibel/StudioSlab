//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"

namespace R2toR {

    inline int compute_index(int i, int j, int N, int M) {
        return (i * M + j);
    }


    void copy_effective_modes(const fftw_complex *output_data, ComplexArray &effective_output_data,
                              int N, int M, Real scaleFactor) {

        int halfN = N / 2;
        int halfM = M / 2;

        std::cout << "FFT Output in wavenumber order:" << std::endl;
        for (int i = -halfN; i < halfN; ++i) {
            for (int j = -halfM; j < halfM; ++j) {
                fix wrapped_i = (i + N) % N;
                fix wrapped_j = (j + M) % M;

                fix out_i = i+halfN;
                fix out_j = j+halfM;

                fix k_in = compute_index(wrapped_i, wrapped_j, N, M);
                fix k_out = out_i*M + out_j;

                // Access the complex value at the computed index

                auto &inVal = output_data[k_in];
                auto &outVal = effective_output_data[k_out];

                outVal = scaleFactor * Complex(inVal[0], inVal[1]);
            }
        }

        /*
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < M; ++j) {
                auto &outVal = effective_output_data[i + j*N];
                auto &inVal = output_data[i*M + j];

                outVal = scaleFactor * Complex(inVal[0], inVal[1]);
            }
        }
         */

        /*
        fix hN = N/2;

        for (int i = 0; i < N; ++i) {
            // fix i_out = i<=N/2 ? i-1+hN : i-1-hN;
            // fix i_out = i;
            for (int j = 0; j < m; ++j) {
                fix k_in = i*m + j;
                fix k_out = i + j*N;

                auto &in = output_data[k_in];
                auto &out = effective_output_data[k_out];

                out = scaleFactor * Complex(in[0], in[1]);
            }
        }*/
    }

    auto moveData(const fftw_complex *in, ComplexArray &out, UInt n, UInt M, Real scaleFactor) {
        if(0)
            for (int j = 0; j < M; ++j) {
                for (int i = 0; i < n; ++i) {
                    fix &zv       = in[i*M + j ];
                    auto &value = out[i*M + j ];
                    value = {zv[0], zv[1]};
                }
            }

        else {
            int halfM = (int)M/2;
            for (int i = 0; i < M; ++i) {
                fix i_out= i; // j<0 ? j+halfM : 0;
                fix i_in = i; // j+halfM; // (j + M) % M;

                for (auto j = 0; j < n; ++j) {
                    fix &z_in = in [j*M + i_in  ];
                    auto &z_out = out[j*M + i_out ];
                    z_out = {z_in[0], z_in[1]};
                }
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
        fix Δk = N * dk; // N/2 not N/2+1
        fix Δω = M * dω;

        auto out = new R2toC::DiscreteFunction(n, M, -Δk, -Δω, 2 * Δk, 2*Δω);
        auto &data_out = out->getData();
        fix scale = 1e0/(N*M);

        moveData(data_dft, data_out, n, M, scale);

        fftw_destroy_plan(plan);
        fftw_free(data_dft);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};
    }


} // R2toR