//
// Created by joao on 5/29/24.
//

#include "R2toRLaplacian.h"

namespace Slab::Math::R2toR {

    Base::NumericFunction<Real2D, Real> &
    R2toRLaplacian::operateAndStoreResult(Base::NumericFunction<Real2D, Real> &output,
                                                 const Base::NumericFunction<Real2D, Real> &funky) const {
        auto &uFunky = static_cast<const NumericFunction&>(funky);
        auto &outputty = static_cast<NumericFunction&>(output);

        return uFunky.Laplacian(outputty);
    }
} // Slab