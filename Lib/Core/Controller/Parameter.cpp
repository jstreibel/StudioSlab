//
// Created by joao on 10/13/21.
//

#include "Parameter.h"

#include "Utils/Utils.h"


namespace Slab::Core {

    FParameter::FParameter(const Str &commandLineArgName, const Str &description)
            : fullCLName(commandLineArgName), description(description) {
        auto split = Common::SplitString(fullCLName, ",", 2);
        shortCLName = split[0];
        longCLName = split.size() > 1 ? split[1] : "";
    }

    auto FParameter::getFullCommandLineName() const -> Str {
        return fullCLName;
    }

    auto FParameter::getCommandLineArgumentName(bool longNameIfPresent) const -> Str {
        return (longNameIfPresent && longCLName != "") ? longCLName : shortCLName;
    }

    auto FParameter::getDescription() const -> Str { return description; }

    bool FParameter::operator<(const FParameter *rhs) {
        return fullCLName < rhs->fullCLName;
    }

    void FParameter::setDescription(Str newDescription) {
        this->description = newDescription;
    }

    bool FParameter::operator==(Str str) const {
        return Common::SplitString(fullCLName, ",")[0] == str;
    }


    OStream &operator<<(OStream &out, const FParameter &b) {
        out << b.ValueToString();
        return out;
    }

    OStream &operator<<(OStream &out, const FParameter *b) {
        out << b->ValueToString();
        return out;
    }

}