//
// Created by joao on 7/19/19.
//

#include "CommonCLParameters.h"

// template<>
// auto ParameterTemplate<bool>::addToOptionsGroup(CLODEasyInit &add) const -> void {
//     add(fullCLName, description, CLOptions::value<bool>());
// }

#ifndef PARAMETER_TEMPLATE_SOURCE_ON_HEADER

#include "Utils/Utils.h"

namespace Slab::Core {

    template<class Type>
    CLParameterTemplate<Type>::~CLParameterTemplate() = default;

    template<class Type>
    auto CLParameterTemplate<Type>::valueToString() const -> std::string {
        return ToStr(val);
    }

    template<class Type>
    auto CLParameterTemplate<Type>::addToCommandLineOptionsGroup(CLODEasyInit &add) const -> void {
        auto value = CLOptions::value<Type>()->default_value(ToStr(val));
        add(fullCLName, description, value);
    }

    template<class Type>
    void CLParameterTemplate<Type>::setValue(const void *pVoid) {
        val = *static_cast<const Type *>(pVoid);
    }

    template<class Type>
    void CLParameterTemplate<Type>::setValue(Type value) { this->val = value; }

    template<class Type>
    const void *CLParameterTemplate<Type>::getValueVoid() const { return &val; }

    template<class Type>
    void CLParameterTemplate<Type>::setValueFromCommandLine(VariableValue var) {
        try {
            this->val = var.as<Type>();
            // std::cout << "Parameter " << commandLineArgName << " being attributed value " << val << " from command line." << std::endl;
        } catch (cxxopts::exceptions::parsing &exception) {
            auto msg = Str("Parameter '") + fullCLName + "' "
                       + "failed conversion from command line input. Type is " + typeid(Type).name()
                       + ". Exception what(): " + exception.what();

            throw Exception(msg);
        }
    }

    template<class Type>
    auto CLParameterTemplate<Type>::getValue() const -> Type {
        return val;
    }

    template<class Type>
    auto CLParameterTemplate<Type>::operator*() -> Type & {
        return val;
    }

    template<class Type>
    auto CLParameterTemplate<Type>::operator*() const -> const Type & {
        return val;
    }

    template<class Type>
    CLParameterTemplate<Type>::operator Type() const {
        return val;
    }

    template<class Type>
    auto CLParameterTemplate<Type>::operator=(const Type &rhs) -> CLParameterTemplate & {
        val = rhs;
        return *this;
    }

    template<class Type>
    auto CLParameterTemplate<Type>::operator=(Type &rhs) -> CLParameterTemplate & {
        val = rhs;
        return *this;
    }

    template<class Type>
    auto CLParameterTemplate<Type>::operator<(const Type &rhs) -> bool {
        return val < rhs;
    }

    template
    class CLParameterTemplate<int>;

    template
    class CLParameterTemplate<Real>;

    template
    class CLParameterTemplate<Str>;

    template
    class CLParameterTemplate<StrVector>;

    template
    class CLParameterTemplate<bool>;

}

#endif

