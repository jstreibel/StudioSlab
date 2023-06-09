#ifndef DEFS_H
#define DEFS_H

//#include <chrono>
#include "Parameter.h"
// #include "ParameterUtils.h"

//#include "Core/Util/Workaround/STDLibInclude.h"
//#include "Controller/Console/CommandLineInterfaceDefinitions.h"


// We extend the std namespace to make sure to_string method also works (the way we want) with strings and bools.
namespace std{
    auto to_string(std::string str) -> String;
    auto to_string(bool val) -> String;
}


template <class Type>
class ParameterTemplate : public Parameter {
    Type val;

public:
    typedef Type MyType;

    typedef std::shared_ptr<ParameterTemplate> Ptr;
    static Ptr New(Type val, const String& argName, const String& descr){
        return std::make_unique<ParameterTemplate>(val, argName, descr);
    }

    ParameterTemplate(Type val, const String& argName, const String& descr);
    ~ParameterTemplate();

    auto valueToString() const -> String override;
    auto getOptionDescription(CLODEasyInit& base) const -> CLODEasyInit & override;

    void setValueFrom(VariableValue var) override;

    void setValue(const void *pVoid) override;
    void setValue(Type value);
    auto getValueVoid() const -> const void * override;
    auto getValue() const -> Type;

    auto operator*() -> Type&;
    auto operator*() const -> const Type&;

    void operator=(const Type rhs);
    operator ParameterTemplate*();
};

template <class Type>
auto operator*(const ParameterTemplate<Type> &p1, const ParameterTemplate<Type> p2) -> Type {
    return p1.val * p2.val;
}


typedef ParameterTemplate < int    > IntegerParameter;
typedef ParameterTemplate < Real > RealParameter;
typedef ParameterTemplate < String > StringParameter;
typedef ParameterTemplate < bool   > BoolParameter;








#endif // DEFS_H
