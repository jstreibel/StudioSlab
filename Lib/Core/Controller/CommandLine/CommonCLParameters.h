#ifndef DEFS_H
#define DEFS_H

#include "Core/Controller/CommandLine/CLParameter.h"

// #define PARAMETER_TEMPLATE_SOURCE_ON_HEADER


namespace Slab::Core {

    template<class Type>
    class CLParameterTemplate : public CLParameter {
    protected:
        Type val;

    public:
        typedef Type MyType;

        typedef std::shared_ptr<CLParameterTemplate> Ptr;

        static Ptr New(Type val, const Str &argName, const Str &descr) {
            return std::make_unique<CLParameterTemplate>(val, argName, descr);
        }

        CLParameterTemplate(Type val, const Str &argName, const Str &descr);

        ~CLParameterTemplate();

        auto valueToString() const -> Str override;

        auto addToCommandLineOptionsGroup(CLODEasyInit &base) const -> void override;

        void setValueFromCommandLine(VariableValue var) override;

        void setValue(const void *pVoid) override;

        void setValue(Type value);

        auto getValueVoid() const -> const void * override;

        auto getValue() const -> Type;

        auto operator*() -> Type &;

        auto operator*() const -> const Type &;

        explicit operator Type() const;

        auto operator=(const Type &rhs) -> CLParameterTemplate &;

        auto operator=(Type &rhs) -> CLParameterTemplate &;

        auto operator=(const CLParameterTemplate &rhs) -> Type;

        auto operator<(const Type &rhs) -> bool;

    };

    template<class Type>
    auto CLParameterTemplate<Type>::operator=(const CLParameterTemplate &rhs) -> Type {
        return *this = rhs;
    }

    template<class Type>
    CLParameterTemplate<Type>::CLParameterTemplate(Type val, const Str &argName, const Str &description)
            : CLParameter(argName, description), val(val) {}

    template<class TypeA, class TypeB>
    auto operator*(const CLParameterTemplate<TypeA> &p1, const CLParameterTemplate<TypeB> &p2) -> TypeA {
        return p1.getValue() * p2.getValue();
    }

    template<class TypeA, class TypeB>
    auto operator*(const CLParameterTemplate<TypeA> &p, const TypeB &val) -> TypeB { return p.getValue() * val; }

    template<class TypeA, class TypeB>
    auto operator*(const TypeB &val, const CLParameterTemplate<TypeA> &p) -> TypeB { return p * val; }


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

    typedef CLParameterTemplate<int> IntegerParameter;
    typedef CLParameterTemplate<Real> RealParameter;
    typedef CLParameterTemplate<StrVector> MultiStringParameter;
    typedef CLParameterTemplate<Str> StringParameter;
    typedef CLParameterTemplate<bool> BoolParameter;


}

#endif // DEFS_H
