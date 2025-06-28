//
// Created by joao on 7/7/24.
//

#ifndef STUDIOSLAB_NATIVEFUNCTION_H
#define STUDIOSLAB_NATIVEFUNCTION_H

#include <functional>

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class NativeFunction : public RtoR::Function {
        DevFloat (*native)(DevFloat);
    public:
        NativeFunction(DevFloat (*native)(DevFloat));

        DevFloat operator()(DevFloat x) const override;

        auto Clone() const -> Pointer<Type> override;
    };

} // Slab::Math::RtoR

#endif //STUDIOSLAB_NATIVEFUNCTION_H
