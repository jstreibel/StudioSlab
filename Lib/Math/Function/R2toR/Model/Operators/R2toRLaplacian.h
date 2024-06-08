//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_R2TORLAPLACIAN_H
#define STUDIOSLAB_R2TORLAPLACIAN_H

#include "../R2toRDiscreteFunction.h"
#include "Math/VectorSpace/Algebra/Operator.h"

namespace Slab::Math::R2toR {

    class R2toRLaplacian : public Operator<Base::DiscreteFunction<Real2D, Real>> {
        using Operator<Base::DiscreteFunction<Real2D, Real>>::operator*;
    protected:

        Base::DiscreteFunction<Real2D, Real> &operateAndStoreResult(Base::DiscreteFunction<Real2D, Real> &output,
                                                                    const Base::DiscreteFunction<Real2D, Real> &input) const override;
    };

} // Slab

#endif //STUDIOSLAB_R2TORLAPLACIAN_H
