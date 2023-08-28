//
// Created by joao on 7/19/19.
//

#include "CommonParameters.h"

// template<>
// auto ParameterTemplate<bool>::addToOptionsGroup(CLODEasyInit &add) const -> void {
//     add(fullCLName, description, CLOptions::value<bool>());
// }

#ifndef PARAMETER_TEMPLATE_SOURCE_ON_HEADER

#include "Utils/Utils.h"

template<class Type>
ParameterTemplate<Type>::~ParameterTemplate() = default;

template<class Type>
auto ParameterTemplate<Type>::valueToString() const -> std::string {
	return ToStr(val);
}

template<class Type>
auto ParameterTemplate<Type>::addToOptionsGroup(CLODEasyInit &add) const -> void {
    auto value = CLOptions::value<Type>()->default_value(ToStr(val));
    add(fullCLName, description, value);
}

template<class Type>
void ParameterTemplate<Type>::setValue(const void *pVoid) {
    val = *static_cast<const Type*>(pVoid);
}

template<class Type>
void ParameterTemplate<Type>::setValue(Type value) { this->val = value; }

template<class Type>
const void *ParameterTemplate<Type>::getValueVoid() const { return &val;    }

template<class Type>
void ParameterTemplate<Type>::setValueFrom(VariableValue var) {
    try {
        this->val = var.as<Type>();
        // std::cout << "Parameter " << commandLineArgName << " being attributed value " << val << " from command line." << std::endl;
    } catch (cxxopts::exceptions::parsing &exception) {
        auto msg = Str("Parameter '") + fullCLName + "' "
             + "failed conversion from command line input. Type is " + typeid(Type).name()
             + ". Exception what(): " + exception.what();

        throw msg;
    }
}

template<class Type>
auto ParameterTemplate<Type>::getValue() const -> Type {
    return val;
}

template<class Type>
auto ParameterTemplate<Type>::operator*() -> Type & {
    return val;
}

template<class Type>
auto ParameterTemplate<Type>::operator*() const -> const Type& {
    return val;
}

template<class Type>
ParameterTemplate<Type>::operator Type() const {
    return val;
}

template<class Type>
auto ParameterTemplate<Type>::operator=(const Type &rhs) -> ParameterTemplate & {
    val = rhs;
    return *this;
}

template<class Type>
auto ParameterTemplate<Type>::operator=(Type &rhs) -> ParameterTemplate & {
    val = rhs;
    return *this;
}

template<class Type>
auto ParameterTemplate<Type>::operator<(const Type &rhs) -> bool {
    return val < rhs;
}

#endif

template class ParameterTemplate<int>;
template class ParameterTemplate<Real>;
template class ParameterTemplate<Str>;
template class ParameterTemplate<bool>;
