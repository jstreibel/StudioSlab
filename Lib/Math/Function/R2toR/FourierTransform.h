//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
#define STUDIOSLAB_FOURIERTRANSFORM_H

#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

namespace Slab::Math::R2toR {

    class FourierTransform {
    public:
        static std::shared_ptr<R2toR::DiscreteFunction> Compute(const R2toR::DiscreteFunction &in);
    };

} // R2toR

#endif //STUDIOSLAB_DISCRETEFOURIERTRANSFORM_H
