#ifndef DEFS_H
#define DEFS_H

#include "Parameter.h"

// We extend the std namespace to make sure to_string method also works (the way we want) with strings and bools.
namespace std{
    auto to_string(std::string str) -> Str;
    auto to_string(bool val) -> Str;
}


template <class Type>
class ParameterTemplate : public Parameter {
    Type val;

public:
    typedef Type MyType;

    typedef std::shared_ptr<ParameterTemplate> Ptr;
    static Ptr New(Type val, const Str& argName, const Str& descr){
        return std::make_unique<ParameterTemplate>(val, argName, descr);
    }

    ParameterTemplate(Type val, const Str& argName, const Str& descr);
    ~ParameterTemplate();

    auto valueToString() const -> Str override;
    auto addToOptionsGroup(CLODEasyInit& base) const -> void override;

    void setValueFrom(VariableValue var) override;

    void setValue(const void *pVoid) override;
    void setValue(Type value);
    auto getValueVoid() const -> const void * override;
    auto getValue() const -> Type;

    auto operator*() -> Type&;
    auto operator*() const -> const Type&;

    operator Type() const;

    auto operator=(const Type &rhs) -> ParameterTemplate&;
    auto operator=(Type &rhs) -> ParameterTemplate&;
    auto operator<(const Type &rhs) -> bool;

};

template <class Type>
auto operator*(const ParameterTemplate<Type> &p1, const ParameterTemplate<Type> &p2) -> Type {
    return p1.val * p2.val;
}

typedef ParameterTemplate < int  > IntegerParameter;
typedef ParameterTemplate < Real > RealParameter;
typedef ParameterTemplate < Str  > StringParameter;
typedef ParameterTemplate < bool > BoolParameter;








#endif // DEFS_H
