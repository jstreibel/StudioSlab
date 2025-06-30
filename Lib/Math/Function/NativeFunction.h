//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_NATIVEFUNCTION_H
#define STUDIOSLAB_NATIVEFUNCTION_H


// #include "Function.h"

namespace Slab::Math::Base {

    template<typename BaseFunction>
    class NativeFunction : public BaseFunction {
        typename BaseFunction::OutCategory (*nativeFunction)(typename BaseFunction::InCategory);

    public:
        explicit NativeFunction(
            typename BaseFunction::OutCategory(* native_function)(typename BaseFunction::InCategory))
            : nativeFunction(native_function)
        {
        }

        typename BaseFunction::OutCategory operator()(typename BaseFunction::InCategory x) const override {
            return nativeFunction(x);
        }

    };
}

#endif //STUDIOSLAB_NATIVEFUNCTION_H
