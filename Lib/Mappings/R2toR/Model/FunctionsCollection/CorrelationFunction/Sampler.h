//
// Created by joao on 22/08/23.
//

#ifndef STUDIOSLAB_SAMPLER_H
#define STUDIOSLAB_SAMPLER_H


#include <vector>

#include "Common/Types.h"
#include "Phys/Formalism/Categories.h"
#include "Mappings/RtoR2/ParametricCurve.h"


namespace R2toR {

    typedef std::vector<Real2D> R2Vec;

    class Sampler {
        Count nSamples=500;
    public:
        explicit Sampler(Count nSamples=500) : nSamples(nSamples) { };

        virtual auto generateSamples() -> R2Vec = 0;

        auto get_nSamples() const   -> Count { return nSamples; }
        auto set_nSamples(Count n)  -> void  { nSamples = n; }

    };

    class Sampler1D : public Sampler {
        RtoR2::ParametricCurve::Ptr parametricCurve;

    public:
        explicit Sampler1D(RtoR2::ParametricCurve::Ptr curve = nullptr);

        auto setCurve(RtoR2::ParametricCurve::Ptr curve) -> void;

        auto generateSamples() -> R2Vec override;
    };

} // R2toR

#endif //STUDIOSLAB_SAMPLER_H
