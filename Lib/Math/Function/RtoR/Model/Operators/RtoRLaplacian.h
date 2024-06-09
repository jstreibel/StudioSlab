//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_RTORLAPLACIAN_H
#define STUDIOSLAB_RTORLAPLACIAN_H

#include "Math/VectorSpace/Algebra/Operator.h"
#include "../RtoRNumericFunction.h"

namespace Slab::Math::RtoR {

    class RtoRLaplacian : public Operator<Base::NumericFunction<Real,Real>> {
    public:
        using Operator<Base::NumericFunction<Real,Real>>::operator*;

    protected:
        Base::NumericFunction<Real, Real> &operateAndStoreResult(Base::NumericFunction<Real, Real> &output,
                                                                 const Base::NumericFunction<Real, Real> &input) const override;
        // NumericFunction &operateAndStoreResult(NumericFunction &output, const NumericFunction &input) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_RTORLAPLACIAN_H
