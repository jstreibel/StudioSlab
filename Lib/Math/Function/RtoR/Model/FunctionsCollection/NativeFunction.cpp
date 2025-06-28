//
// Created by joao on 7/7/24.
//

#include "NativeFunction.h"

namespace Slab::Math::RtoR {

    NativeFunction::NativeFunction(DevFloat (*function)(DevFloat))
    : native(function) {    }

    DevFloat NativeFunction::operator()(DevFloat x) const { return (*native)(x); }

    Pointer<Base::FunctionT<DevFloat, DevFloat>::Type> NativeFunction::Clone() const {
        return New<NativeFunction>(this->native);
    }


} // Slab::Math::RtoR