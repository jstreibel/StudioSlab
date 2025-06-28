#ifndef DEFS_H
#define DEFS_H

#include "Core/Controller/CommandLine/CommandLineParameter.h"

// #define PARAMETER_TEMPLATE_SOURCE_ON_HEADER


namespace Slab::Core {

    template<class Type>
    class TCommandLineParameter : public FCommandLineParameter {
    protected:
        Type val;

    public:
        typedef Type MyType;

        typedef std::shared_ptr<TCommandLineParameter> Ptr;

        static Ptr New(Type Val, const Str &ArgName, const Str &Descr) {
            return std::make_unique<TCommandLineParameter>(Val, ArgName, Descr);
        }

        TCommandLineParameter(Type Value, const Str &ArgName, const Str &ShortDescription);

        ~TCommandLineParameter();

        [[nodiscard]] auto ValueToString() const -> Str override;

        auto AddToCommandLineOptionsGroup(CLODEasyInit &base) const -> void override;

        void SetValueFromCommandLine(VariableValue var) override;

        void SetValue(const void *pVoid) override;

        void SetValue(Type value);

        [[nodiscard]] auto GetValueVoid() const -> const void * override;

        auto GetValue() const -> Type;

        auto operator*() -> Type &;

        auto operator*() const -> const Type &;

        explicit operator Type() const;

        auto operator=(const Type &rhs) -> TCommandLineParameter &;

        auto operator=(Type &rhs) -> TCommandLineParameter &;

        auto operator=(const TCommandLineParameter &rhs) -> Type;

        auto operator<(const Type &rhs) -> bool;

    };

    template<class Type>
    auto TCommandLineParameter<Type>::operator=(const TCommandLineParameter &rhs) -> Type {
        return *this = rhs;
    }

    template<class Type>
    TCommandLineParameter<Type>::TCommandLineParameter(Type Value, const Str &ArgName, const Str &ShortDescription)
            : FCommandLineParameter(ArgName, ShortDescription), val(Value) {}

    template<class TypeA, class TypeB>
    auto operator*(const TCommandLineParameter<TypeA> &p1, const TCommandLineParameter<TypeB> &p2) -> TypeA {
        return p1.GetValue() * p2.GetValue();
    }

    template<class TypeA, class TypeB>
    auto operator*(const TCommandLineParameter<TypeA> &p, const TypeB &val) -> TypeB { return p.GetValue() * val; }

    template<class TypeA, class TypeB>
    auto operator*(const TypeB &val, const TCommandLineParameter<TypeA> &p) -> TypeB { return p * val; }


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

    typedef TCommandLineParameter<int> IntegerParameter;
    typedef TCommandLineParameter<DevFloat> RealParameter;
    typedef TCommandLineParameter<StrVector> MultiStringParameter;
    typedef TCommandLineParameter<Str> StringParameter;
    typedef TCommandLineParameter<bool> BoolParameter;


}

#endif // DEFS_H
