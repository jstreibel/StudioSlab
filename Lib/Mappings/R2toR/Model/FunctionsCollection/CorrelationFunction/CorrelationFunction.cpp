//
// Created by joao on 22/08/23.
//

#include "CorrelationFunction.h"

#include <utility>
#include "Sampler.h"


R2toR::CorrelationFunction::CorrelationFunction(R2toR::Function::ConstPtr baseFunc, std::shared_ptr<Sampler> sampler)
: baseFunction( std::move(baseFunc) )
, sampler(      std::move(sampler) )
{   }

Real R2toR::CorrelationFunction::operator()(Real2D x) const {
    if(baseFunction == nullptr) return 0;

    auto samples = sampler->getSamples();

    IN func = *baseFunction;

    Real av_s₁s₂ = 0.0;
    Real av_s₁ = 0.0;
    Real av_s₂ = 0.0;
    for(IN s : samples){
        Real2D x_s₁ = s;
        Real2D x_s₂ = s+x;

        IN s₁ = func(x_s₁);
        IN s₂ = func(x_s₂);

        av_s₁s₂ += s₁*s₂;
        av_s₁ += s₁;
        av_s₂ += s₂;
    }

    IN n = (Real)samples.size();

    return av_s₁s₂/n - av_s₁*av_s₂/(n*n);
}

void R2toR::CorrelationFunction::setBaseFunction(R2toR::Function::ConstPtr baseFunc) {
    baseFunction = std::move(baseFunc);
}

void R2toR::CorrelationFunction::setSampler(std::shared_ptr<Sampler> _sampler) {
    this->sampler = std::move(_sampler);

}


