//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "../CLDefs.h"

#include <Utils/Types.h>

class Parameter {
protected:
    Str shortCLName, longCLName, fullCLName, description;

public:
    typedef std::shared_ptr<Parameter> Ptr;
    typedef std::shared_ptr<const Parameter> ConstPtr;

    Parameter(const Str& commandLineArgName, const Str& description);

    virtual auto addToOptionsGroup(CLODEasyInit &group) const -> void = 0;

    virtual auto valueToString() const -> Str = 0;

    virtual void setValueFrom(VariableValue var) = 0;
    virtual void setValue(const void*) = 0;
    virtual auto getValueVoid() const -> const void* = 0;

    auto getFullCLName() const -> Str;
    auto getCLName(bool longNameIfPresent=false) const -> Str;
    auto getDescription() const -> Str;
    void setDescription(Str newDescription);

    bool operator<(const Parameter *rhs);

    bool operator==(Str str) const;

    bool operator!=(const Parameter &rhs) const;

};

OStream & operator << (OStream &out, const Parameter &b);
OStream & operator << (OStream &out, const Parameter *b);

#endif //FIELDS_PARAMETERBASE_H
