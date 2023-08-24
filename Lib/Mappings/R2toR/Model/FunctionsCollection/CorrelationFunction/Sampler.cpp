//
// Created by joao on 22/08/23.
//

#include "Sampler.h"
#include "Common/RandUtils.h"

#include <utility>

namespace R2toR {

    Sampler::Sampler(Count nSamples) : nSamples(nSamples) { }

    auto Sampler::invalidateSamples() -> void { valid = false; }

    auto Sampler::getSamples() -> R2Vec {
        if(!valid){
            samples = generateSamples();
            valid = true;
        }
        return samples;
    }

    auto Sampler::get_nSamples() const -> Count { return nSamples; }

    auto Sampler::set_nSamples(Count n) -> void {
        if(n != nSamples) invalidateSamples();

        nSamples = n;
    }



    Sampler1D::Sampler1D(RtoR2::ParametricCurve::Ptr curve) : parametricCurve(std::move(curve)) { }

    auto Sampler1D::setCurve(RtoR2::ParametricCurve::Ptr curve) -> void { parametricCurve = std::move(curve); }

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


    RandomSampler::RandomSampler(Real2D min, Real2D max, Count nSamples)
    : Sampler(nSamples) , xMin(min.x), xMax(max.x) , yMin(min.y), yMax(max.y) { }

    R2Vec RandomSampler::generateSamples() {
        using namespace RandUtils;

        auto samples = R2toR::R2Vec(get_nSamples());
        for(OUT s : samples) s = {random(xMin, xMax), random(yMin, yMax)};

        return samples;
    }

} // R2toR