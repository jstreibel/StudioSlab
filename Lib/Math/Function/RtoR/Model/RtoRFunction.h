//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONRTOR_H
#define V_SHAPE_FUNCTIONRTOR_H

#include "Utils/Types.h"

#include "Utils/NativeTypes.h"
#include "Math/Function/CommonFunctions.h"

namespace Slab::Math::RtoR {
    typedef Base::Functions<DevFloat>::Function1D Function;
    typedef Base::Functions<DevFloat>::Function1DSummable FunctionSummable;

    DefinePointers(Function)
    DefinePointers(FunctionSummable)
}

/*
class Function : public FunctionsCollection<Real>::Function1D {
public:
    virtual ~Function() = default;
    virtual Real operator()(Real x) const = 0;
};
*/

#endif //V_SHAPE_FUNCTIONRTOR_H
