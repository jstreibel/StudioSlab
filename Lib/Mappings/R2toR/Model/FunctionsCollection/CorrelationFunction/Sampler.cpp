//
// Created by joao on 22/08/23.
//

#include "Sampler.h"

#include <utility>

namespace R2toR {

    Sampler1D::Sampler1D(RtoR2::ParametricCurve::Ptr curve)
    : parametricCurve(std::move(curve)) {

    }

    auto Sampler1D::setCurve(RtoR2::ParametricCurve::Ptr curve) -> void {
        parametricCurve = std::move(curve);
    }

    R2Vec Sampler1D::generateSamples() {
        if(parametricCurve== nullptr) return {};

        auto &curve = *parametricCurve;

        fix n = get_nSamples();
        auto samples = R2toR::R2Vec();
        auto Δs = curve.getΔs();
        auto sMin = curve.get_sMin();
        for(auto i=0; i<n; ++i) {
            fix sNorm = i/(Real)n;
            fix s = sMin + sNorm*Δs;

            samples.emplace_back(curve(s));
        }

        return samples;
    }


} // R2toR