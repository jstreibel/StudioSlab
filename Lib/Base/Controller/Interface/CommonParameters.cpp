//
// Created by joao on 7/19/19.
//

#include "Common/STDLibInclude.h"
#include "CommonParameters.h"
#include "Common/Log/Log.h"

auto std::to_string(String str) -> String { return String("\"") + str + String("\""); }
auto std::to_string(bool val) -> String {        return val ? "True" : "False"; }


template<class Type>
ParameterTemplate<Type>::ParameterTemplate(Type val, const String& argName, const String& descr)
    : Parameter(argName, descr), val(val) {}

template<class Type>
ParameterTemplate<Type>::~ParameterTemplate() = default;

template<class Type>
auto ParameterTemplate<Type>::valueToString() const -> std::string {
	return std::to_string(val);
}

template<class Type>
auto ParameterTemplate<Type>::addToOptionsGroup(CLODEasyInit &add) const -> void {
    auto value = CLOptions::value<Type>()->default_value(ToString(val));
    add(commandLineArgName, description, value);
}

template<>
auto ParameterTemplate<bool>::addToOptionsGroup(CLODEasyInit &add) const -> void {
    add(commandLineArgName, description, CLOptions::value<bool>());
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
        auto msg = String("Parameter '") + commandLineArgName + "' failed conversion from command line input. Type is " + typeid(Type).name();
        Log::Error() << msg << Log::Flush;
        throw exception;
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

template class ParameterTemplate<int>;
template class ParameterTemplate<Real>;
template class ParameterTemplate<String>;
template class ParameterTemplate<bool>;

