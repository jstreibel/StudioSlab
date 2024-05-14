//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"

namespace R2toR {


    void copy_effective_modes(const fftw_complex *output_data, ComplexArray &effective_output_data,
                              int N, int m, Real scaleFactor) {

        fix hN = N/2;

        for (int i = 0; i < N; ++i) {
            fix i_out = i<=N/2 ? i-1+hN : i-1-hN;
            for (int j = 0; j < m; ++j) {
                fix k_in = i*m + j;
                fix k_out = i_out + j*N;

                auto &in = output_data[k_in];
                auto &out = effective_output_data[k_out];

                out = scaleFactor * Complex(in[0], in[1]);
            }
        }
    }

    auto R2toRDFT::DFT(const DiscreteFunction &in) -> std::shared_ptr<R2toC::DiscreteFunction> {
        fix _N_ = in.getN();
        fix _M_ = in.getM();

        if(_N_%2 || _M_%2) throw Exception("can't FT real data with odd number of sites");

        fix N = _N_%2==0 ? _N_ : _N_-1;
        fix M = _M_%2==0 ? _M_ : _M_-1;
        fix Lx = in.getDomain().getLx();
        fix Ly = in.getDomain().getLy();

        // fix n = N;
        fix m = M/2+1;
        fix dk = 2*M_PI/Lx;
        fix dω = 2*M_PI/Ly;
        fix Δk = N*dk; // N/2 not N/2+1
        fix Δω = m*dω;


        auto out = new R2toC::DiscreteFunction(N, m, -.5*Δk, 0, Δk, Δω);

        // Allocate input data (real) and output data (complex)
        auto* input_data/*(N * M)*/ = const_cast<Real*>(&in.getSpace().getHostData(false)[0]);
        auto* output_data = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * m);

        auto plan = fftw_plan_dft_r2c_2d(N, M, input_data, output_data, FFTW_ESTIMATE);

        fftw_execute(plan);

        auto &out_effective_data = out->getData();
        fix scale = 1e0/(N*m);
        copy_effective_modes(output_data, out_effective_data, N, m, scale);

        fftw_destroy_plan(plan);
        fftw_free(output_data);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};
    }


} // R2toR