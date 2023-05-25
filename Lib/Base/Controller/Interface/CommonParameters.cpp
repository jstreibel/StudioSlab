//
// Created by joao on 7/19/19.
//

#include <Common/Workaround/StringStream.h>
#include "Common/STDLibInclude.h"
#include "CommonParameters.h"

#include <utility>
#include <typeinfo>


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
auto ParameterTemplate<Type>::getOptionDescription(CLODEasyInit &base) const -> CLODEasyInit & {
    return base(commandLineArgName.c_str(), po::value<Type>()->default_value(val), description.c_str());
}

template<>
auto ParameterTemplate<bool>::getOptionDescription(CLODEasyInit &base) const -> CLODEasyInit & {
    return base(commandLineArgName.c_str(), po::bool_switch(), description.c_str());
}

template<class Type>
void ParameterTemplate<Type>::setValue(const void *pVoid) {
    val = *static_cast<const Type*>(pVoid);
}

template<class Type>
const void *ParameterTemplate<Type>::getValue() const {
    return &val;
}

template<class Type>
void ParameterTemplate<Type>::setValueFrom(VariableValue var) {
    try {
        this->val = var.as<Type>();
        // std::cout << "Parameter " << commandLineArgName << " being attributed value " << val << " from command line." << std::endl;
    } catch (boost::bad_any_cast &exception) {
        auto msg = String("Parameter '") + commandLineArgName + "' failed conversion from program_options::variable_value";
        std::cout << msg << ". Type is " << typeid(Type).name() << std::endl;
        throw exception;
    }
}

template<class Type>
auto ParameterTemplate<Type>::value() const -> Type {
    return val;
}

template<class Type>
auto ParameterTemplate<Type>::operator*() -> Type & {
    return val;
}

template<class Type>
auto ParameterTemplate<Type>::operator*() const -> Type {
    return val;
}

template<class Type>
ParameterTemplate<Type>::operator ParameterTemplate *() {
    return this;
}


template class ParameterTemplate<int>;
template class ParameterTemplate<double>;
template class ParameterTemplate<String>;
template class ParameterTemplate<bool>;
