//
// Created by joao on 10/13/21.
//

#include "CLParameter.h"

#include "Utils/Utils.h"


namespace Slab::Core {

    CLParameter::CLParameter(const Str &commandLineArgName, const Str &description)
            : fullCLName(commandLineArgName), description(description) {
        auto split = Common::SplitString(fullCLName, ",", 2);
        shortCLName = split[0];
        longCLName = split.size() > 1 ? split[1] : "";
    }

    auto CLParameter::getFullCommandLineName() const -> Str {
        return fullCLName;
    }

    auto CLParameter::getCommandLineArgumentName(bool longNameIfPresent) const -> Str {
        return (longNameIfPresent && longCLName != "") ? longCLName : shortCLName;
    }

    auto CLParameter::getDescription() const -> Str { return description; }

    bool CLParameter::operator<(const CLParameter *rhs) {
        return fullCLName < rhs->fullCLName;
    }

    void CLParameter::setDescription(Str newDescription) {
        this->description = newDescription;
    }

    bool CLParameter::operator==(Str str) const {
        return Common::SplitString(fullCLName, ",")[0] == str;
    }


    OStream &operator<<(OStream &out, const CLParameter &b) {
        out << b.valueToString();
        return out;
    }

    OStream &operator<<(OStream &out, const CLParameter *b) {
        out << b->valueToString();
        return out;
    }

}