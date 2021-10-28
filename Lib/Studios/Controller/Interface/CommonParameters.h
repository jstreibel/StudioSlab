#ifndef DEFS_H
#define DEFS_H

#include <chrono>
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
public:
    typedef Type MyType;

    ParameterTemplate(Type val, const String& argName, const String& descr);
    ~ParameterTemplate();

    auto valueToString() const -> String override;
    auto getOptionDescription(CLODEasyInit& base) const -> CLODEasyInit & override;

    void setValueFrom(VariableValue var) override;

    void setValue(const void *pVoid) override;
    auto getValue() const -> const void * override;
    auto value() const -> Type;

    auto operator*() const -> Type;

private:
    Type val;
};


typedef ParameterTemplate<int> IntegerParameter;
typedef ParameterTemplate<double> DoubleParameter;
typedef ParameterTemplate<String> StringParameter;
typedef ParameterTemplate<bool> BoolParameter;








#endif // DEFS_H
