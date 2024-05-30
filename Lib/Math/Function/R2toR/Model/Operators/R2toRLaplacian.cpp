//
// Created by joao on 5/29/24.
//

#include "R2toRLaplacian.h"

namespace Slab {

    Math::Base::DiscreteFunction<Math::Real2D, Real> &
    Math::R2toR::R2toRLaplacian::operateAndStoreResult(Math::Base::DiscreteFunction<Math::Real2D, Real> &output,
                                                 const Math::Base::DiscreteFunction<Math::Real2D, Real> &funky) const {
        auto &uFunky = static_cast<const R2toR::DiscreteFunction&>(funky);
        auto &outputty = static_cast<R2toR::DiscreteFunction&>(output);

        return uFunky.Laplacian(outputty);
    }
} // Slab