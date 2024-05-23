//
// Created by joao on 13/05/24.
//

#include "R2toC_to_R2toR.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"

namespace Slab::Math {

    R2toR::DiscreteFunction_ptr
    Convert(const R2toC::DiscreteFunction_constptr &in, Math::R2toC_to_R2toR_Mode mode) {
        fix N = in->N,
                M = in->M;
        fix Lx = in->Lx,
                Ly = in->Ly;
        fix xMin = in->x0;
        fix yMin = in->y0;

        auto out = Slab::New<R2toR::DiscreteFunction_CPU>(N, M, xMin, yMin, Lx / N, Ly / M);

        switch (mode) {
            case Phase:
                return ConvertToPhase(in, out);
            case Magnitude:
                return ConvertToAbs(in, out);
            case PowerSpectrum:
                return ConvertToPowerSpectrum(in, out);
            case RealPart:
                return ConvertToReal(in, out);
            case ImaginaryPart:
                return ConvertToImaginary(in, out);
        }

        throw Exception("unknown Complex conversion mode");
    }

    R2toR::DiscreteFunction_ptr
    ConvertToAbs(const R2toC::DiscreteFunction_constptr &in, R2toR::DiscreteFunction_ptr out) {

        assert(in->N == out->getN() && in->M == out->getM());

        auto &v_in = in->getData();
        auto &v_out = out->getSpace().getHostData(false);

        fix n = in->N * in->M;
        for (auto i = 0; i < n; ++i) {
            fix &z = v_in[i];
            fix r = sqrt(norm(z));
            auto &v = v_out[i];

            v = r;
        }

        return out;
    }

    R2toR::DiscreteFunction_ptr ConvertToPowerSpectrum(const R2toC::DiscreteFunction_constptr &in,
                                                             R2toR::DiscreteFunction_ptr out) {
        assert(in->N == out->getN() && in->M == out->getM());

        auto &v_in = in->getData();
        auto &v_out = out->getSpace().getHostData(false);

        fix n = in->N * in->M;
        for (auto i = 0; i < n; ++i) {
            fix &z = v_in[i];
            fix r = norm(z);
            auto &v = v_out[i];

            v = r;
        }

        return out;
    }

    R2toR::DiscreteFunction_ptr
    ConvertToPhase(const R2toC::DiscreteFunction_constptr &in, R2toR::DiscreteFunction_ptr out) {
        assert(in->N == out->getN() && in->M == out->getM());

        auto &v_in = in->getData();
        auto &v_out = out->getSpace().getHostData(false);

        fix n = in->N * in->M;
        for (auto i = 0; i < n; ++i)
            v_out[i] = arg(v_in[i]);

        return out;
    }

    R2toR::DiscreteFunction_ptr
    ConvertToReal(const R2toC::DiscreteFunction_constptr &in, R2toR::DiscreteFunction_ptr out) {
        assert(in->N == out->getN() && in->M == out->getM());

        auto &v_in = in->getData();
        auto &v_out = out->getSpace().getHostData(false);

        fix n = in->N * in->M;
        for (auto i = 0; i < n; ++i)
            v_out[i] = v_in[i].real();

        return out;
    }

    R2toR::DiscreteFunction_ptr
    ConvertToImaginary(const R2toC::DiscreteFunction_constptr &in, R2toR::DiscreteFunction_ptr out) {
        assert(in->N == out->getN() && in->M == out->getM());

        auto &v_in = in->getData();
        auto &v_out = out->getSpace().getHostData(false);

        fix n = in->N * in->M;
        for (auto i = 0; i < n; ++i)
            v_out[i] = v_in[i].imag();

        return out;
    }


}