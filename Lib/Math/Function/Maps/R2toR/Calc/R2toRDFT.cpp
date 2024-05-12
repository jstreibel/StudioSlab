//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"

namespace R2toR {


    void copy_effective_modes(const fftw_complex *output_data, ComplexArray &effective_output_data, int N, int M) {
        int nRows = N / 2 + 1;
        int nCols = M / 2 + 1;

        for (int i = 0; i < nRows; ++i) {
            for (int j = 0; j < nCols; ++j) {
                auto &outVal = effective_output_data[i * nCols + j];
                auto &inVal = output_data[i * nCols + j];

                outVal = {inVal[0], inVal[1]};
            }
        }
    }

    auto R2toRDFT::DFT(const DiscreteFunction &in) -> std::shared_ptr<R2toC::DiscreteFunction> {
        fix N = in.getN()%2==0 ? in.getN() : in.getN()-1;
        fix M = in.getM()%2==0 ? in.getM() : in.getM()-1;
        fix Lx = in.getDomain().getLx();
        fix Ly = in.getDomain().getLy();

        fix n = N/2+1;
        fix m = M/2+1;
        fix dk = 2*M_PI/Lx;
        fix dω = 2*M_PI/Ly;

        auto out = new R2toC::DiscreteFunction(n, m, 0, 0, n*dk, m*dω);

        // Allocate input data (real) and output data (complex)
        auto* input_data/*(N * M)*/ = const_cast<Real*>(&in.getSpace().getHostData(false)[0]);
        auto* output_data = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N * (M / 2 + 1));

        fftw_plan_dft_r2c_2d(N, M, input_data, output_data, FFTW_ESTIMATE);

        copy_effective_modes(output_data, out->space.getData(), N, M);

        return std::shared_ptr<R2toC::DiscreteFunction>{out};
    }


} // R2toR