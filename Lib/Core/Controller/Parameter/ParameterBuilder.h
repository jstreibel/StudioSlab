//
// Created by joao on 8/26/25.
//

#ifndef PARAMETERBUILDER_H
#define PARAMETERBUILDER_H

#include "BuiltinParameters.h"

namespace Slab::Core
{


    inline TPointer<RealParameter> MakePositiveRealParameter(
        DevFloat BaseValue,
        const FParameterDescription& ParameterDescription,
        bool bIncludeZero, DevFloat eps=1.e-3)
    {
        RealParameter::TParameterAttributes Attributes;
        Attributes.OtherAttributes[CMinimumTag] = 0.0;
        Attributes.Validator = [bIncludeZero](const RealParameter::MyType &Value)
        {
            if (bIncludeZero) return Value >= 0.0;
            return Value > 0.0;
        };
        Attributes.Filter = [bIncludeZero, eps](const RealParameter::MyType &Value)
        {
            if (Value < 0.0) return bIncludeZero ? 0.0 : eps;

            return Value;
        };

        return New<RealParameter>(BaseValue, ParameterDescription, Attributes);
    }

    struct FIntegerParameterOptions
    {
        std::optional<int> MinVal;
        std::optional<int> MaxVal;

        bool bIncludeZero   =true;
        bool bEven          =false;

    };

    inline TPointer<IntegerParameter> MakeIntParam(
        Int BaseValue,
        const FParameterDescription& ParameterDescription,
        FIntegerParameterOptions Options)
    {
        using FAttributes = IntegerParameter::TParameterAttributes;
        auto Param = New<IntegerParameter>(BaseValue, ParameterDescription, FAttributes{});

        auto &Attributes = Param->GetAttributes();

        if (Options.MinVal) Attributes.OtherAttributes[CMinimumTag] = Options.MinVal.value();
        if (Options.MaxVal) Attributes.OtherAttributes[CMaximumTag] = Options.MaxVal.value();

        Attributes.Validator =
            [Param](const IntegerParameter::MyType &Value)
            {
                auto& Attr = Param->GetAttributes();
                fix MinOpt = Attr.OtherAttributes[CMinimumTag];
                fix MaxOpt = Attr.OtherAttributes[CMaximumTag];

                if (MinOpt && MaxOpt) return Value >= MinOpt.value() && Value <= MaxOpt.value();

                if (MinOpt) return Value >= MinOpt.value();

                if (MaxOpt) return Value <= MaxOpt.value();

                if (Contains(Attr.ExcludedValues, Value)) return false;

                return true;
            };

        fix bEven = Options.bEven;
        Attributes.Filter =
            [Param, BaseValue, bEven](const IntegerParameter::MyType &Value)
            {
                auto& Attr = Param->GetAttributes();
                fix MinOpt = Attr.OtherAttributes[CMinimumTag];
                fix MaxOpt = Attr.OtherAttributes[CMaximumTag];

                if (MinOpt && Value<MinOpt.value()) return MinOpt.value();
                if (MaxOpt && Value>MaxOpt.value()) return MaxOpt.value();
                if (Contains(Attr.ExcludedValues, Value)) return BaseValue;

                if (bEven && Value%2) return Value+1;

                return Value;
            };

        return Param;
    }
}

#endif //PARAMETERBUILDER_H

