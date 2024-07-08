//
// Created by joao on 7/7/24.
//

#ifndef STUDIOSLAB_NATIVEFUNCTION_H
#define STUDIOSLAB_NATIVEFUNCTION_H

#include <functional>

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class NativeFunction : public RtoR::Function {
        Real (*native)(Real);
    public:
        NativeFunction(Real (*native)(Real));

        Real operator()(Real x) const override;

        auto Clone() const -> Pointer<Type> override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_NATIVEFUNCTION_H
