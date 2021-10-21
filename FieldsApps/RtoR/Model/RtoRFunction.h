//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONRTOR_H

#include "Model/FunctionsBase.h"

#include "Model/math-util.h"
#include "Model/MathTypes.h"

namespace RtoR {
    typedef Base::Functions<Real>::Function1D Function;
    typedef Base::Functions<Real>::Function1DSummable FunctionSummable;
}

/*
class Function : public FunctionsCollection<Real>::Function1D {
public:
    virtual ~Function() = default;
    virtual Real operator()(Real x) const = 0;
};
*/

#endif //V_SHAPE_FUNCTIONRTOR_H
