//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_R2TORLAPLACIAN_H
#define STUDIOSLAB_R2TORLAPLACIAN_H

#include "../R2toRNumericFunction.h"
#include "Math/VectorSpace/Algebra/Operator.h"

namespace Slab::Math::R2toR {

    class R2toRLaplacian : public Operator<Base::NumericFunction<Real2D, DevFloat>> {
        using Operator<Base::NumericFunction<Real2D, DevFloat>>::operator*;
    protected:

        Base::NumericFunction<Real2D, DevFloat> &operateAndStoreResult(Base::NumericFunction<Real2D, DevFloat> &output,
                                                                   const Base::NumericFunction<Real2D, DevFloat> &input) const override;
    };

} // Slab

#endif //STUDIOSLAB_R2TORLAPLACIAN_H
