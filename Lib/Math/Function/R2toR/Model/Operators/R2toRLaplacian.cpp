//
// Created by joao on 5/29/24.
//

#include "R2toRLaplacian.h"

namespace Slab::Math::R2toR {

    Base::NumericFunction<Real2D, DevFloat> &
    R2toRLaplacian::operateAndStoreResult(Base::NumericFunction<Real2D, DevFloat> &output,
                                                 const Base::NumericFunction<Real2D, DevFloat> &funky) const {
        auto &uFunky = static_cast<const FNumericFunction&>(funky);
        auto &outputty = static_cast<FNumericFunction&>(output);

        return uFunky.Laplacian(outputty);
    }
} // Slab