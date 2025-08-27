//
// Created by joao on 8/26/25.
//

#include "ParameterBuilder.h"

Slab::TPointer<Slab::Core::TParameter<double>>
Slab::Core::MakeRealParam(
    DevFloat BaseValue,
    const FParameterDescription& ParameterDescription,
    const FRealParameterOptions& Options)
{

    auto Param = New<RealParameter>(BaseValue, ParameterDescription, RealParameter::TParameterAttributes());
    auto& Attributes = Param->GetAttributes();

    if (Options.MinVal) Attributes.BuiltinAttributes[CMinimumTag] = Options.MinVal;
    if (Options.MaxVal) Attributes.BuiltinAttributes[CMaximumTag] = Options.MaxVal;

    Attributes.Filter = [Param](const RealParameter::MyType &Value)
    {
        auto& Attr = Param->GetAttributes();
        fix MinOpt = Attr.BuiltinAttributes[CMinimumTag];
        fix MaxOpt = Attr.BuiltinAttributes[CMaximumTag];

        if (MinOpt && Value<MinOpt.value()) return MinOpt.value();
        if (MaxOpt && Value>MaxOpt.value()) return MaxOpt.value();

        return Value;
    };

    return Param;
}

Slab::TPointer<Slab::Core::TParameter<int>>
Slab::Core::MakeIntParam(
    Int BaseValue,
    const FParameterDescription& ParameterDescription,
    const FIntegerParameterOptions& Options)
{
    using FAttributes = IntegerParameter::TParameterAttributes;
    auto Param = New<IntegerParameter>(BaseValue, ParameterDescription, FAttributes{});

    auto &Attributes = Param->GetAttributes();

    if (Options.MinVal) Attributes.BuiltinAttributes[CMinimumTag] = Options.MinVal.value();
    if (Options.MaxVal) Attributes.BuiltinAttributes[CMaximumTag] = Options.MaxVal.value();

    Attributes.Validator =
        [Param](const IntegerParameter::MyType &Value)
        {
            auto& Attr = Param->GetAttributes();
            fix MinOpt = Attr.BuiltinAttributes[CMinimumTag];
            fix MaxOpt = Attr.BuiltinAttributes[CMaximumTag];

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
            fix MinOpt = Attr.BuiltinAttributes[CMinimumTag];
            fix MaxOpt = Attr.BuiltinAttributes[CMaximumTag];

            if (MinOpt && Value<MinOpt.value()) return MinOpt.value();
            if (MaxOpt && Value>MaxOpt.value()) return MaxOpt.value();
            if (Contains(Attr.ExcludedValues, Value)) return BaseValue;

            if (bEven && Value%2) return Value+1;

            return Value;
        };

    return Param;
}
