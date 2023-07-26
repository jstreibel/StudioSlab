//
// Created by joao on 10/13/21.
//

#include "Parameter.h"

#include "Common/Utils.h"



Parameter::Parameter(const Str& commandLineArgName, const Str& description)
    : fullCLName(commandLineArgName), description(description)
{
    auto split = Common::SplitString(fullCLName, ",", 2);
    shortCLName = split[0];
    longCLName = split.size()>1 ? split[1] : "";
}

auto Parameter::getFullCLName() const -> Str {
    return fullCLName;
}

auto Parameter::getCLName(bool longNameIfPresent) const -> Str {

    return (longNameIfPresent && longCLName!="") ? longCLName : shortCLName;
}

auto Parameter::getDescription() const -> Str {return description; }

bool Parameter::operator<(const Parameter *rhs) {
    return fullCLName < rhs->fullCLName;
}

void Parameter::setDescription(Str newDescription) {
    this->description = newDescription;
}

bool Parameter::operator==(Str str) const {
    return Common::SplitString(fullCLName, ",")[0] == str;
}


OStream & operator << (OStream &out, const Parameter &b)
{
    out << b.valueToString();
    return out;
}

OStream & operator << (OStream &out, const Parameter *b)
{
    out << b->valueToString();
    return out;
}
