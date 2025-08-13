//
// Created by joao on 22/08/23.
//

#ifndef STUDIOSLAB_CORRELATIONFUNCTION_H
#define STUDIOSLAB_CORRELATIONFUNCTION_H


#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Sampler.h"

namespace Slab::Math::R2toR {

    class CorrelationFunction : public R2toR::Function {
        R2toR::Function_constptr baseFunction;
        Sampler_ptr sampler;

    public:
        CorrelationFunction() = default;
        CorrelationFunction(R2toR::Function_constptr baseFunc, Sampler_ptr sampler);

        void setBaseFunction(R2toR::Function_constptr baseFunc);
        void setSampler(Sampler_ptr sampler);

        DevFloat operator()(Real2D x) const override;
    };

}


#endif //STUDIOSLAB_CORRELATIONFUNCTION_H
