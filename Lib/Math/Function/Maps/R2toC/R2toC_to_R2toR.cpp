//
// Created by joao on 13/05/24.
//

#include "R2toC_to_R2toR.h"
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"

std::shared_ptr<R2toR::DiscreteFunction>
Math::Convert(std::shared_ptr<const R2toC::DiscreteFunction> in, Math::R2toC_to_R2toR_Mode mode) {
    fix N = in->N,
        M = in->M;
    fix Lx = in->Lx,
        Ly = in->Ly;
    fix xMin = in->x0;
    fix yMin = in->y0;

    auto out = std::make_shared<R2toR::DiscreteFunction_CPU>(N, M, xMin, yMin, Lx/N, Ly/M);

    switch (mode) {
        case Phase:
            return ConvertToPhase(in, out);
        case Magnitude:
            return ConvertToAbs(in, out);
    }

    throw Exception("unknown Complex conversion mode");
}

std::shared_ptr<R2toR::DiscreteFunction>
Math::ConvertToAbs(std::shared_ptr<const R2toC::DiscreteFunction> in, std::shared_ptr<R2toR::DiscreteFunction> out) {

    assert(in->N == out->getN() && in->M == out->getM());

    auto &v_in = in->getData();
    auto &v_out = out->getSpace().getHostData(false);

    fix n = in->N*in->M;
    for(auto i=0; i<n; ++i) {
        fix &z = v_in[i];
        fix r = abs(z);
        auto &v = v_out[i];

        v = r;
    }

    return out;
}

std::shared_ptr<R2toR::DiscreteFunction>
Math::ConvertToPhase(std::shared_ptr<const R2toC::DiscreteFunction> in, std::shared_ptr<R2toR::DiscreteFunction> out) {
    assert(in->N == out->getN() && in->M == out->getM());

    auto &v_in = in->getData();
    auto &v_out = out->getSpace().getHostData(false);

    fix n = in->N*in->M;
    for(auto i=0; i<n; ++i)
        v_out[i] = arg(v_in[i]);

    return out;
}
