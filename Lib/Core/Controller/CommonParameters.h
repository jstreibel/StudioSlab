#ifndef DEFS_H
#define DEFS_H

#include "Parameter.h"

// #define PARAMETER_TEMPLATE_SOURCE_ON_HEADER


namespace Slab::Core {

    template<class Type>
    class TParameter : public FParameter {
    protected:
        Type Value;

    public:
        typedef Type MyType;

        TParameter(Type Value, const FParameterDescription& ParameterDescription);

        ~TParameter() override;

        [[nodiscard]] auto ValueToString() const -> Str override;

        auto AddToCommandLineOptionsGroup(CLODEasyInit &base) const -> void override;

        void SetValueFromCommandLine(VariableValue var) override;

        void SetValue(const void *pVoid) override;

        void SetValue(Type value);

        [[nodiscard]] auto GetValueVoid() const -> const void * override;

        auto GetValue() const -> Type;

        auto GetValue() -> Type&;

        auto operator*() -> Type &;

        auto operator*() const -> const Type &;

        explicit operator Type() const;

        auto operator=(const Type &rhs) -> TParameter &;

        auto operator=(Type &rhs) -> TParameter &;

        auto operator=(const TParameter &rhs) -> Type;

        auto operator<(const Type &rhs) -> bool;

    };

    template<class Type>
    auto TParameter<Type>::operator=(const TParameter &rhs) -> Type {
        return *this = rhs;
    }

    template<class Type>
    TParameter<Type>::TParameter(Type Value, const FParameterDescription& ParameterDescription)
    : FParameter(ParameterDescription), Value(Value) {}

    template<class TypeA, class TypeB>
    auto operator*(const TParameter<TypeA> &p1, const TParameter<TypeB> &p2) -> TypeA {
        return p1.GetValue() * p2.GetValue();
    }

    template<class TypeA, class TypeB>
    auto operator*(const TParameter<TypeA> &p, const TypeB &val) -> TypeB { return p.GetValue() * val; }

    template<class TypeA, class TypeB>
    auto operator*(const TypeB &val, const TParameter<TypeA> &p) -> TypeB { return p * val; }


#ifdef PARAMETER_TEMPLATE_SOURCE_ON_HEADER

#include "Utils/STDLibInclude.h"
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
    auto ParameterTemplate<Type>::getValue() const -> Type {    return val;}

    template<class Type>
    auto ParameterTemplate<Type>::operator*() -> Type & {    return val;}

    template<class Type>
    auto ParameterTemplate<Type>::operator*() const -> const Type& {    return val; }

    template<class Type>
    ParameterTemplate<Type>::operator Type() const {    return val; }

    template<class Type>
    auto ParameterTemplate<Type>::operator=(const Type &rhs) -> ParameterTemplate & {    val = rhs;    return *this;    }

    template<class Type>
    auto ParameterTemplate<Type>::operator=(Type &rhs) -> ParameterTemplate & {    val = rhs;    return *this;  }

    template<class Type>
    auto ParameterTemplate<Type>::operator=(const ParameterTemplate &rhs) -> Type {
        return rhs.val;
    }

    template<class Type>
    auto ParameterTemplate<Type>::operator<(const Type &rhs) -> bool {    return val < rhs; }


#endif // PARAMETER_TEMPLATE_SOURCE_ON_HEADER

    typedef TParameter<int> IntegerParameter;
    typedef TParameter<DevFloat> RealParameter;
    typedef TParameter<StrVector> MultiStringParameter;
    typedef TParameter<Str> StringParameter;
    typedef TParameter<bool> BoolParameter;

    class EnumParameter final : public IntegerParameter
    {
    public:
        EnumParameter(int Value, FParameterDescription ParameterDescription, const StrVector& values)
            : TParameter<int>(Value, ParameterDescription),
              Values(values)
        {
        }

    private:
        StrVector Values;
    };
}

#endif // DEFS_H
