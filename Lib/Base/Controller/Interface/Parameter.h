//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "../CLDefs.h"

#include <Common/Types.h>

class Parameter {

public:
    Parameter(const String& commandLineArgName, const String& description);

    virtual auto getOptionDescription(CLODEasyInit &base) const -> CLODEasyInit & = 0;

    virtual auto valueToString() const -> String = 0;

    virtual void setValueFrom(VariableValue var) = 0;
    virtual void setValue(const void*) = 0;
    virtual auto getValueVoid() const -> const void* = 0;

    auto getCommandLineArgName(bool clean=false) const -> String;
    auto getDescription() const -> String;
    void setDescription(String newDescription);

    bool operator<(const Parameter *rhs);

    bool operator==(String name) const;

    bool operator!=(const Parameter &rhs) const;

protected:
    String commandLineArgName, description;
};

std::ostream & operator << (std::ostream &out, const Parameter &b);
std::ostream & operator << (std::ostream &out, const Parameter *b);

#endif //FIELDS_PARAMETERBASE_H
