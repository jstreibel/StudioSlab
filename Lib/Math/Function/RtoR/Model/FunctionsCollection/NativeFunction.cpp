//
// Created by joao on 7/7/24.
//

#include "NativeFunction.h"

namespace Slab::Math::RtoR {

    NativeFunction::NativeFunction(std::function<DevFloat(DevFloat)> function)
    : Function(function) {    }

    DevFloat NativeFunction::operator()(DevFloat x) const { return Function(x); }

    TPointer<Base::FunctionT<DevFloat, DevFloat>::Type> NativeFunction::Clone() const {
        return New<NativeFunction>(this->Function);
    }


} // Slab::Math::RtoR