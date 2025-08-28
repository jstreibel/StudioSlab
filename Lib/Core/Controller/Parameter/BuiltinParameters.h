#ifndef DEFS_H
#define DEFS_H

#include <utility>

#include "Parameter.h"

namespace Slab::Core {

    constexpr auto CMinimumTag = "Minimum";
    constexpr auto CMaximumTag = "Maximum";
    constexpr auto CExcludeZeroTag = "ExcludeZero";

    template<class Type>
    class TParameter : public FParameter {

    public:
        typedef Type MyType;

        struct TParameterAttributes
        {
            Vector<MyType>                        ExcludedValues;
            std::function<bool(const MyType&)>    Validator = [](const MyType&) { return true; } ;
            std::function<MyType(const MyType&)>  Filter    = [](const MyType& UnfilteredValue) { return UnfilteredValue; } ;
            std::map<Str, std::optional<MyType>>  BuiltinAttributes;
        };

        TParameter(Type Value, const FParameterDescription& ParameterDescription, const TParameterAttributes& Attributes={});
        ~TParameter() override;

        EParameterType GetType() const override;

        [[nodiscard]] auto ValueToString() const -> Str override;

        auto AddToCommandLineOptionsGroup(CLODEasyInit &base) const -> void override;

        void SetValueFromCommandLine     (VariableValue var) override;

        void SetValue                    (const void *pVoid) override;

        void SetValue                    (Type value);

        [[nodiscard]] auto GetValueVoid() const -> const void * override;

        [[nodiscard]] auto GetValue() const -> Type;

        auto GetValue() -> Type&;

        auto operator*() -> Type &;

        auto operator*() const -> const Type &;

        explicit operator Type() const;

        auto operator=(const Type &rhs) -> TParameter &;

        auto operator=(const TParameter& rhs) -> Type&;

        auto operator<(const Type &rhs) -> bool;

        auto GetAttributes() const -> const TParameterAttributes& { return Attributes; };
        auto GetAttributes()       ->       TParameterAttributes& { return Attributes; };

    protected:
        Type Value;
        TParameterAttributes Attributes;
    };

    template<class TypeA, class TypeB>
    auto operator*(const TParameter<TypeA> &p1, const TParameter<TypeB> &p2) -> TypeA { return p1.GetValue() * p2.GetValue(); }

    template<class TypeA, class TypeB>
    auto operator*(const TParameter<TypeA> &p, const TypeB &val) -> TypeB { return p.GetValue() * val; }

    template<class TypeA, class TypeB>
    auto operator*(const TypeB &val, const TParameter<TypeA> &p) -> TypeB { return p * val; }

    typedef TParameter<int>       IntegerParameter;
    typedef TParameter<DevFloat>  RealParameter;
    typedef TParameter<StrVector> MultiStringParameter;
    typedef TParameter<Str>       StringParameter;
    typedef TParameter<bool>      BoolParameter;

    /*
    class RealParameter final : public TParameter<DevFloat>
    {
    public:
        RealParameter(DevFloat Value, const FParameterDescription& ParameterDescription, const TParameterAttributes& Attributes={})
        : TParameter(Value, ParameterDescription, Attributes) { }

        ~RealParameter() override = default;

        RealParameter& operator=(DevFloat x)
        {
            TParameter::operator=(x);
            return *this;
        };
    };
    */


}

#endif // DEFS_H
