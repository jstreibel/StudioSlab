//
// Created by joao on 8/26/25.
//

#ifndef PARAMETERBUILDER_H
#define PARAMETERBUILDER_H

#include "BuiltinParameters.h"

namespace Slab::Core
{
    struct FRealParameterOptions
    {
        std::optional<DevFloat> MinVal;
        std::optional<DevFloat> MaxVal;
    };

    TPointer<RealParameter> MakeRealParam(
        DevFloat BaseValue,
        const FParameterDescription& ParameterDescription,
        const FRealParameterOptions& Options);

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
        const FIntegerParameterOptions& Options);
}

#endif //PARAMETERBUILDER_H

