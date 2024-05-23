//
// Created by joao on 06/05/24.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONRENDERER_H
#define STUDIOSLAB_R2TORFUNCTIONRENDERER_H

#include <memory>
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

namespace Slab::Graphics {

    using namespace Math;
    
    class R2toRFunctionRenderer {
    public:
        static auto renderToDiscrete(const R2toR::Function& function,
                                     Resolution x, Resolution y, R2toR::Domain,
                                     Real scale=1.0)
                                     -> R2toR::DiscreteFunction_ptr;

        static auto renderToDiscrete(const R2toR::Function& in,
                                     R2toR::DiscreteFunction_ptr out, Real scale=1.0)
                                     -> R2toR::DiscreteFunction_ptr ;
    };

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONRENDERER_H
