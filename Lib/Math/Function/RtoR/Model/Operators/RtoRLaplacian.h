//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_RTORLAPLACIAN_H
#define STUDIOSLAB_RTORLAPLACIAN_H

#include "Math/VectorSpace/Operator.h"
#include "../RtoRDiscreteFunction.h"

namespace Slab::Math::RtoR {

    class RtoRLaplacian : public Operator<Base::DiscreteFunction<Real,Real>> {
    public:
        using Operator<Base::DiscreteFunction<Real,Real>>::operator*;

    protected:
        Base::DiscreteFunction<Real, Real> &operateAndStoreResult(Base::DiscreteFunction<Real, Real> &output,
                                                                  const Base::DiscreteFunction<Real, Real> &input) const override;
        // DiscreteFunction &operateAndStoreResult(DiscreteFunction &output, const DiscreteFunction &input) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_RTORLAPLACIAN_H
