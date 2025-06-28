//
// Created by joao on 22/08/23.
//

#ifndef STUDIOSLAB_SAMPLER_H
#define STUDIOSLAB_SAMPLER_H


#include <vector>

#include "Utils/Types.h"
#include "Math/Formalism/Categories.h"
#include "Math/Function/RtoR2/ParametricCurve.h"


namespace Slab::Math::R2toR {

    typedef Vector<Real2D> R2Vec;


    class Sampler {
        CountType nSamples=500;
        bool valid = false;

        R2Vec samples;

        virtual auto generateSamples() -> R2Vec = 0;


    public:
        explicit Sampler(CountType nSamples=500);;

        auto getSamples() -> R2Vec;

        auto invalidateSamples() -> void;

        auto get_nSamples() const   -> CountType;
        auto set_nSamples(CountType n)  -> void;

    };


    class Sampler1D : public Sampler {
        RtoR2::ParametricCurve::Ptr parametricCurve;

        auto generateSamples() -> R2Vec override;

    public:
        explicit Sampler1D(RtoR2::ParametricCurve::Ptr curve = nullptr);

        auto setCurve(RtoR2::ParametricCurve::Ptr curve) -> void;

    };


    class RandomSampler : public Sampler {
        DevFloat xMin, xMax, yMin, yMax;
        auto generateSamples() -> R2Vec override;

    public:
        RandomSampler(Real2D min, Real2D max, CountType nSamples=500);

    };

    DefinePointers(Sampler)

    DefinePointers(Sampler1D)

} // R2toR

#endif //STUDIOSLAB_SAMPLER_H
