//
// Created by joao on 22/08/23.
//

#ifndef STUDIOSLAB_CORRELATIONFUNCTION_H
#define STUDIOSLAB_CORRELATIONFUNCTION_H


#include "Mappings/R2toR/Model/R2toRFunction.h"
#include "Common/SharedPointer.h"
#include "Sampler.h"

namespace R2toR {

    class CorrelationFunction : public R2toR::Function {
        R2toR::Function::ConstPtr baseFunction;
        std::shared_ptr<Sampler> sampler;

    public:
        CorrelationFunction() = default;
        CorrelationFunction(R2toR::Function::ConstPtr baseFunc, std::shared_ptr<Sampler> sampler);

        void setBaseFunction(R2toR::Function::ConstPtr baseFunc);
        void setSampler(std::shared_ptr<Sampler> sampler);

        Real operator()(Real2D x) const override;
    };

}


#endif //STUDIOSLAB_CORRELATIONFUNCTION_H
