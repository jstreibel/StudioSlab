//
// Created by joao on 7/19/19.
//

#include "BuiltinParameters.h"

#include <utility>

// template<>
// auto ParameterTemplate<bool>::addToOptionsGroup(CLODEasyInit &add) const -> void {
//     add(fullCLName, description, CLOptions::value<bool>());
// }

#include "Utils/Utils.h"

namespace Slab::Core {

    template<class Type>      TParameter<Type>::~TParameter() = default;
    template<class Type> auto TParameter<Type>::ValueToString() const -> std::string { return ToStr(Value); }
    template<class Type> auto TParameter<Type>::AddToCommandLineOptionsGroup(CLODEasyInit &add) const -> void
    {
        auto value = CLOptions::value<Type>()->default_value(ToStr(Value));
        add(GetFullCommandLineName(), GetDescription(), value);
    }

    template<class Type>       void TParameter<Type>::SetValue(const void *pVoid)
    {
        fix Val = *static_cast<const Type *>(pVoid);

        Value = Attributes.Filter(Val);
    }
    template<class Type>       void TParameter<Type>::SetValue(Type value)
    {
        this->Value = Attributes.Filter(std::move(value));
    }
    template<class Type> const void*TParameter<Type>::GetValueVoid() const { return &Value; }
    template<class Type>       void TParameter<Type>::SetValueFromCommandLine(VariableValue var) {
        try {
            this->Value = var.as<Type>();
            // std::cout << "Parameter " << commandLineArgName << " being attributed value " << val << " from command line." << std::endl;
        } catch (cxxopts::exceptions::parsing &exception) {
            auto msg = Str("Parameter '") + GetFullCommandLineName() + "' "
                       + "failed conversion from command line input. Type is " + typeid(Type).name()
                       + ". Exception what(): " + exception.what();

            throw Exception(msg);
        }
    }

    template<class Type> auto TParameter<Type>::GetValue()      const -> Type  { return Value; }
    template<class Type> auto TParameter<Type>::GetValue()            -> Type& { return Value; }

    template<class Type>
    TParameter<Type>::TParameter(Type Value, const FParameterDescription& ParameterDescription, const TParameterAttributes& Attributes)
    : FParameter(ParameterDescription), Value(Value), Attributes(Attributes)
    {
        if (!Attributes.Validator(Value)) {
            throw std::invalid_argument("Invalid Value " + ToStr(Value) + " to Parameter '" + ParameterDescription.Name + "'");
        }
    }

    template<class Type> auto TParameter<Type>::operator*()           -> Type & { return Value; }
    template<class Type> auto TParameter<Type>::operator*()     const -> const Type & { return Value; }
    template<class Type>      TParameter<Type>::operator Type() const { return Value; }
    template<class Type> auto TParameter<Type>::operator=(const Type &      rhs) -> TParameter & { Value = rhs; return *this; }
    template<class Type> auto TParameter<Type>::operator=(const TParameter &rhs) -> Type &       { Value = rhs.Value; return Value; }
    template<class Type> auto TParameter<Type>::operator<(const Type &rhs) -> bool { return Value < rhs; }

    template class TParameter<int>;
    template class TParameter<DevFloat>;
    template class TParameter<Str>;
    template class TParameter<StrVector>;
    template class TParameter<bool>;

}

