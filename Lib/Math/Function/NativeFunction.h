//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_NATIVEFUNCTION_H
#define STUDIOSLAB_NATIVEFUNCTION_H


#include "Function.h"

namespace Slab::Math::Base {

    template<typename BaseFunction>
    class NativeFunction : public BaseFunction {
        BaseFunction::OutCategory (*nativeFunction)(BaseFunction::InCategory);

    public:
        explicit NativeFunction(BaseFunction::OutCategory (*nativeFunction)(typename BaseFunction::InCategory))
        : BaseFunction(nullptr, false)
        , nativeFunction(nativeFunction) {}

        BaseFunction::OutCategory operator()(BaseFunction::InCategory x) const override {
            return nativeFunction(x);
        }

    };
}

#endif //STUDIOSLAB_NATIVEFUNCTION_H
