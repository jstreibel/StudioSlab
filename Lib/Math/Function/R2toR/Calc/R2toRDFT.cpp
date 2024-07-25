//
// Created by joao on 10/05/24.
//

#include <fftw3.h>
#include "R2toRDFT.h"

#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"

// #define DONT_FIX_OUTPUT
#include "FFTWDataMangling.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#define TO_FFTW(a) (reinterpret_cast<fftw_complex*>(a))
#define TO_STD(a) (reinterpret_cast<Complex*>(a))

namespace Slab::Math::R2toR {

    DataMoveMangleMode resolveInputPolicy(R2toRDFT::DataPolicy inputPolicy, R2toRDFT::Transform transform) {
        if(inputPolicy == R2toRDFT::Auto)
            return transform==R2toRDFT::InverseFourier ? DataMoveMangleMode::Mangle : KeepArrangement;

        return inputPolicy==R2toRDFT::Mangle ? DataMoveMangleMode::Mangle : DataMoveMangleMode::KeepArrangement;
    }

    DataMoveMangleMode resolveOutputPolicy(R2toRDFT::DataPolicy outputPolicy, R2toRDFT::Transform transform) {
        if(outputPolicy == R2toRDFT::Auto)
            return transform==R2toRDFT::Fourier ? DataMoveMangleMode::Mangle : KeepArrangement;

        return outputPolicy==R2toRDFT::Mangle ? DataMoveMangleMode::Unmangle : DataMoveMangleMode::KeepArrangement;
    }

    auto R2toRDFT::DFTComplex(const R2toC::NumericFunction &in, Transform transform,
                              DataPolicy inputPolicy, DataPolicy outputPolicy) -> FFTData {
        fix M = (int)in.M;
        fix N = (int)in.N;

        // const_cast because fftw does not accept constant data sources.
        auto* data_src_raw /*(N * M)*/ = TO_FFTW(const_cast<Complex*>(&in.getData()[0]));
        auto* data_src = fftw_alloc_complex(sizeof(fftw_complex) * N * M);
        auto* data_dft = fftw_alloc_complex(sizeof(fftw_complex) * N * M);



        // sign==1 => only mangle if inverse FT

        MoveData(TO_STD(data_src_raw),
                TO_STD(data_src),
                N, M, 1.0,
                 resolveInputPolicy(inputPolicy, transform));

        auto plan = fftw_plan_dft_2d(M, N, TO_FFTW(data_src), data_dft, transform, FFTW_ESTIMATE);

        fftw_execute(plan);

        fix Lₓ = in.Lx;
        fix Lₜ = in.Ly;

        fix dk = M_PI/Lₓ; // 2kₘₐₓ/N;
        fix dω = M_PI/Lₜ;
        fix kₘₐₓ = dk * N; //  2π/Lₓ×N/2 if N even and 2π/Lₓ×(N-1)/2 if N odd
        fix ωₘₐₓ = dω * M; //  same as line above

        auto out = new R2toC::NumericFunction(N, M, -kₘₐₓ, -ωₘₐₓ, 2*kₘₐₓ, 2*ωₘₐₓ);
        auto data_out = &out->getData()[0];
        fix scale = Lₓ*Lₜ/(N*M);

        MoveData(TO_STD(data_dft),
                 TO_STD(data_out),
                 N, M,
                 scale, resolveOutputPolicy(outputPolicy, transform));

        fftw_destroy_plan(plan);
        fftw_free(data_src);
        fftw_free(data_dft);

        return FFTData{out};
    }

    auto R2toRDFT::DFTReal(const NumericFunction &in, Transform transform,
                           DataPolicy inputPolicy, DataPolicy outputPolicy) -> FFTData {
        fix M = (int)in.getM();
        fix N = (int)in.getN();

        // const_cast because fftw does not accept constant data sources.
        auto* data_src_raw /*(N * M)*/ = const_cast<Real*>(&in.getSpace().getHostData(false)[0]);
        auto* data_src = fftw_alloc_complex(sizeof(fftw_complex) * N * M);
        auto* data_dft = fftw_alloc_complex(sizeof(fftw_complex) * N * M);



        // -----

        MoveData(data_src_raw,
                 TO_STD(data_src),
                 N, M, 1.0,
                 resolveInputPolicy(inputPolicy, transform));

        auto plan = fftw_plan_dft_2d(M, N, data_src, data_dft, transform, FFTW_ESTIMATE);

        fftw_execute(plan);

        fix Lₓ = in.getDomain().getLx();
        fix Lₜ = in.getDomain().getLy();

        fix dk = M_PI/Lₓ; // 2kₘₐₓ/N;
        fix dω = M_PI/Lₜ;
        fix kₘₐₓ = dk * N; //  2π/Lₓ×N/2 if N even and 2π/Lₓ×(N-1)/2 if N odd
        fix ωₘₐₓ = dω * M; //  same as line above

        auto out = new R2toC::NumericFunction(N, M, -kₘₐₓ, -ωₘₐₓ, 2*kₘₐₓ, 2*ωₘₐₓ);
        auto data_out = &out->getData()[0];
        fix scale = Lₓ*Lₜ/(N*M);

        MoveData( TO_STD(data_dft),
                 TO_STD(data_out),
                 N, M, scale,
                  resolveOutputPolicy(outputPolicy, transform));

        fftw_destroy_plan(plan);
        fftw_free(data_src);
        fftw_free(data_dft);

        return FFTData{out};
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

    auto R2toRDFT::DFTReal_symmetric(const NumericFunction &toTransform) -> FFTData {
        if(toTransform.getN()%2) throw Exception("can't FT real data with odd number of sites in space dimension");

        NOT_IMPLEMENTED

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
        fix dk = M_PI / Lx;
        fix dω = M_PI / Ly;
        fix Δk = (Real)N * dk;
        fix Δω = M * dω;

        auto out = new R2toC::NumericFunction(n, M, 0, -Δω/2, Δk+dk, Δω);
        auto &data_out = out->getData();
        fix scale = 1e0/(N*M);

        moveData(data_dft, data_out, n, M, scale);

        fftw_destroy_plan(plan);
        fftw_free(data_dft);

        return FFTData{out};
    }

    auto
    R2toRDFT::SpaceDFTReal(const NumericFunction &in) -> R2toRDFT::FFTData {
        fix N = in.getN();
        fix M = in.getM();

        if(N%2) throw Exception("Space DFT for N%2!=0 is not implemented.");

        fix Δt = in.getDomain().getLy();
        fix t_min = in.getDomain().yMin;

        fix L = in.getDomain().getLx();
        fix x_min = in.getDomain().xMin;
        fix x_max = in.getDomain().xMax;
        fix dk = 2*M_PI / L;
        fix Δk = dk*N;
        fix k_min = -Δk*.5;

        fix N_dft = N/2+1;
        auto space_dft_result = New<R2toC::NumericFunction>(N, M, k_min, t_min, Δk, Δt);

        RtoR::NumericFunction_CPU temp_slice (N, x_min, x_max);

        IN data = in.getSpace().getHostData(true);
        OUT data_in = temp_slice.getSpace().getHostData(true);

        for(int j=0; j<M; ++j) {
            memcpy(&data_in[0], &data[0 + j*N], N*sizeof(Real));
            auto result = RtoR::DFT::Compute(temp_slice);

            IN re_pts = result.re->getPoints();
            IN im_pts = result.im->getPoints();

            auto *data_out = &space_dft_result->getData()[0 + j*N];
            for(int i=0; i<re_pts.size(); ++i){
                Complex a = {re_pts[i].y, im_pts[i].y};

                data_out[N/2 + i - 1] = a;
                data_out[N/2 - i    ] = conj(a);
            }
        }

        return space_dft_result;
    }

} // R2toR