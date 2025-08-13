//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_RTORLAPLACIAN_H
#define STUDIOSLAB_RTORLAPLACIAN_H

#include "Math/VectorSpace/Algebra/Operator.h"
#include "../RtoRNumericFunction.h"

namespace Slab::Math::RtoR {

    class RtoRLaplacian : public Operator<Base::NumericFunction<DevFloat,DevFloat>> {
    public:
        using Operator<Base::NumericFunction<DevFloat,DevFloat>>::operator*;

    protected:
        Base::NumericFunction<DevFloat, DevFloat> &operateAndStoreResult(Base::NumericFunction<DevFloat, DevFloat> &output,
                                                                 const Base::NumericFunction<DevFloat, DevFloat> &input) const override;
        // NumericFunction &operateAndStoreResult(NumericFunction &output, const NumericFunction &input) const override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_RTORLAPLACIAN_H
