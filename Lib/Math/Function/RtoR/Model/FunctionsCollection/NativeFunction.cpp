//
// Created by joao on 7/7/24.
//

#include "NativeFunction.h"

namespace Slab::Math::RtoR {

    NativeFunction::NativeFunction(Real (*function)(Real))
    : native(function) {    }

    Real NativeFunction::operator()(Real x) const { return (*native)(x); }

    Pointer<Base::FunctionT<Real, Real>::Type> NativeFunction::Clone() const {
        return New<NativeFunction>(this->native);
    }


} // Slab::Math::RtoR