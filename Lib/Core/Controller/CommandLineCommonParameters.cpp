//
// Created by joao on 7/19/19.
//

#include "CommandLineCommonParameters.h"

// template<>
// auto ParameterTemplate<bool>::addToOptionsGroup(CLODEasyInit &add) const -> void {
//     add(fullCLName, description, CLOptions::value<bool>());
// }

#ifndef PARAMETER_TEMPLATE_SOURCE_ON_HEADER

#include "Utils/Utils.h"

namespace Slab::Core {

    template<class Type>
    TCommandLineParameter<Type>::~TCommandLineParameter() = default;

    template<class Type>
    auto TCommandLineParameter<Type>::ValueToString() const -> std::string {
        return ToStr(val);
    }

    template<class Type>
    auto TCommandLineParameter<Type>::AddToCommandLineOptionsGroup(CLODEasyInit &add) const -> void {
        auto value = CLOptions::value<Type>()->default_value(ToStr(val));
        add(fullCLName, description, value);
    }

    template<class Type>
    void TCommandLineParameter<Type>::SetValue(const void *pVoid) {
        val = *static_cast<const Type *>(pVoid);
    }

    template<class Type>
    void TCommandLineParameter<Type>::SetValue(Type value) { this->val = value; }

    template<class Type>
    const void *TCommandLineParameter<Type>::GetValueVoid() const { return &val; }

    template<class Type>
    void TCommandLineParameter<Type>::SetValueFromCommandLine(VariableValue var) {
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
    auto TCommandLineParameter<Type>::GetValue() const -> Type {
        return val;
    }

    template<class Type>
    auto TCommandLineParameter<Type>::operator*() -> Type & {
        return val;
    }

    template<class Type>
    auto TCommandLineParameter<Type>::operator*() const -> const Type & {
        return val;
    }

    template<class Type>
    TCommandLineParameter<Type>::operator Type() const {
        return val;
    }

    template<class Type>
    auto TCommandLineParameter<Type>::operator=(const Type &rhs) -> TCommandLineParameter & {
        val = rhs;
        return *this;
    }

    template<class Type>
    auto TCommandLineParameter<Type>::operator=(Type &rhs) -> TCommandLineParameter & {
        val = rhs;
        return *this;
    }

    template<class Type>
    auto TCommandLineParameter<Type>::operator<(const Type &rhs) -> bool {
        return val < rhs;
    }

    template
    class TCommandLineParameter<int>;

    template
    class TCommandLineParameter<DevFloat>;

    template
    class TCommandLineParameter<Str>;

    template
    class TCommandLineParameter<StrVector>;

    template
    class TCommandLineParameter<bool>;

}

#endif

