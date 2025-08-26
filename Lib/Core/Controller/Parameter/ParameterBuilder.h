//
// Created by joao on 8/26/25.
//

#ifndef PARAMETERBUILDER_H
#define PARAMETERBUILDER_H

#include "BuiltinParameters.h"

namespace Slab::Core
{
    TPointer<RealParameter> MakePositiveRealParameter(
        DevFloat BaseValue,
        const FParameterDescription& ParameterDescription,
        bool bIncludeZero, DevFloat eps=1.e-3);

    struct FIntegerParameterOptions
    {
        std::optional<int> MinVal;
        std::optional<int> MaxVal;

        bool bIncludeZero   =true;
        bool bEven          =false;

    };

    TPointer<IntegerParameter> MakeIntParam(
        Int BaseValue,
        const FParameterDescription& ParameterDescription,
        FIntegerParameterOptions Options);
}

#endif //PARAMETERBUILDER_H

