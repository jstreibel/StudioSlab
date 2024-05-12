//
// Created by joao on 22/08/23.
//

#ifndef STUDIOSLAB_SAMPLER_H
#define STUDIOSLAB_SAMPLER_H


#include <vector>

#include "Utils/Types.h"
#include "Math/Formalism/Categories.h"
#include "Math/Function/Maps/RtoR2/ParametricCurve.h"


namespace R2toR {

    typedef std::vector<Real2D> R2Vec;


    class Sampler {
        Count nSamples=500;
        bool valid = false;

        R2Vec samples;

        virtual auto generateSamples() -> R2Vec = 0;


    public:
        explicit Sampler(Count nSamples=500);;

        auto getSamples() -> R2Vec;

        auto invalidateSamples() -> void;

        auto get_nSamples() const   -> Count;
        auto set_nSamples(Count n)  -> void;

    };


    class Sampler1D : public Sampler {
        RtoR2::ParametricCurve::Ptr parametricCurve;

        auto generateSamples() -> R2Vec override;

    public:
        explicit Sampler1D(RtoR2::ParametricCurve::Ptr curve = nullptr);

        auto setCurve(RtoR2::ParametricCurve::Ptr curve) -> void;

    };


    class RandomSampler : public Sampler {
        Real xMin, xMax, yMin, yMax;
        auto generateSamples() -> R2Vec override;

    public:
        RandomSampler(Real2D min, Real2D max, Count nSamples=500);

    };

} // R2toR

#endif //STUDIOSLAB_SAMPLER_H
