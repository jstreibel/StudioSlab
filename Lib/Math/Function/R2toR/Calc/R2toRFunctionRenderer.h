//
// Created by joao on 06/05/24.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONRENDERER_H
#define STUDIOSLAB_R2TORFUNCTIONRENDERER_H

#include <memory>
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math::R2toR {
    
    class R2toRFunctionRenderer {
    public:
        static auto renderToDiscrete(const R2toR::Function& function,
                                     Resolution x, Resolution y, R2toR::Domain,
                                     DevFloat scale=1.0)
                                     -> R2toR::NumericFunction_ptr;

        static auto renderToDiscrete(const R2toR::Function& in,
                                     R2toR::NumericFunction_ptr out, DevFloat scale=1.0)
                                     -> R2toR::NumericFunction_ptr ;
    };

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONRENDERER_H
