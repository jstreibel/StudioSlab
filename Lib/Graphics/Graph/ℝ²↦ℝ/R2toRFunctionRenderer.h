//
// Created by joao on 06/05/24.
//

#ifndef STUDIOSLAB_R2TORFUNCTIONRENDERER_H
#define STUDIOSLAB_R2TORFUNCTIONRENDERER_H

#include <memory>
#include "Math/Function/Maps/R2toR/Model/R2toRDiscreteFunction.h"

namespace Graphics {

    class R2toRFunctionRenderer {
    public:
        static auto renderToDiscrete(const R2toR::Function& function,
                                     Resolution x, Resolution y, R2toR::Domain,
                                     Real scale=1.0)
                                     -> std::shared_ptr<R2toR::DiscreteFunction>;

        static auto renderToDiscrete(const R2toR::Function& in,
                                     std::shared_ptr<R2toR::DiscreteFunction>out, Real scale=1.0)
                                     -> std::shared_ptr<R2toR::DiscreteFunction>;
    };

} // Graphics

#endif //STUDIOSLAB_R2TORFUNCTIONRENDERER_H
