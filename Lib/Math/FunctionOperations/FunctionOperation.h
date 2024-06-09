//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_FUNCTIONOPERATION_H
#define STUDIOSLAB_FUNCTIONOPERATION_H

#include "Utils/Pointer.h"
#include "Math/Function/Function.h"

namespace Slab::Math::Base {

    template<class InputFunctionCategory, class OutputFunctionCategory>
    class FunctionOperation {
        static_assert(IsSlabFunction_v<InputFunctionCategory>,
                "InputFunctionCategory must be an instance of Foo");
        static_assert(IsSlabFunction_v<OutputFunctionCategory>,
                "OutputFunctionCategory must be an instance of Foo");

    public:
        Pointer<OutputFunctionCategory> operator[] (Pointer<InputFunctionCategory>) = 0;
    };

} // Slab::Math::Base

#endif //STUDIOSLAB_FUNCTIONOPERATION_H
