//
// Created by joao on 4/09/23.
//

#ifndef STUDIOSLAB_NATIVEFUNCTION_H
#define STUDIOSLAB_NATIVEFUNCTION_H


#include "Function.h"

namespace Core {

    template<typename InputCategory, typename OutputCategory>
    class NativeFunction : public FunctionT<InputCategory,OutputCategory> {
        OutputCategory (*nativeFunction)(InputCategory);

    public:
        explicit NativeFunction(OutputCategory (*nativeFunction)(InputCategory))
        : FunctionT<InputCategory,OutputCategory>(nullptr, false)
        , nativeFunction(nativeFunction) {}

        OutputCategory operator()(InputCategory x) const override {
            return nativeFunction(x);
        }

    };
}

#endif //STUDIOSLAB_NATIVEFUNCTION_H
