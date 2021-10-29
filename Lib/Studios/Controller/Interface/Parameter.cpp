//
// Created by joao on 10/13/21.
//

#include "Parameter.h"



Parameter::Parameter(const String& commandLineArgName, const String& description)
    : commandLineArgName(commandLineArgName), description(description)
{
}

auto Parameter::getCommandLineArgName(bool clean) const -> String {
    const auto s = commandLineArgName;
    const auto separator = ',';

    return clean ? s.substr(0, s.find(separator)) : s;
}
auto Parameter::getDescription() const -> String {return description; }

bool Parameter::operator<(const Parameter *rhs) {
    return commandLineArgName < rhs->commandLineArgName;
}

void Parameter::setDescription(String newDescription) {
    this->description = newDescription;
}

std::ostream & operator << (std::ostream &out, const Parameter &b)
{
    out << b.valueToString();
    return out;
}

std::ostream & operator << (std::ostream &out, const Parameter *b)
{
    out << b->valueToString();
    return out;
}
