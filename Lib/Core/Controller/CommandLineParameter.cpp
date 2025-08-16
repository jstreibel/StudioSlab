//
// Created by joao on 10/13/21.
//

#include "CommandLineParameter.h"

#include "Utils/Utils.h"


namespace Slab::Core {

    FCommandLineParameter::FCommandLineParameter(const Str &commandLineArgName, const Str &description)
            : fullCLName(commandLineArgName), description(description) {
        auto split = Common::SplitString(fullCLName, ",", 2);
        shortCLName = split[0];
        longCLName = split.size() > 1 ? split[1] : "";
    }

    auto FCommandLineParameter::getFullCommandLineName() const -> Str {
        return fullCLName;
    }

    auto FCommandLineParameter::getCommandLineArgumentName(bool longNameIfPresent) const -> Str {
        return (longNameIfPresent && longCLName != "") ? longCLName : shortCLName;
    }

    auto FCommandLineParameter::getDescription() const -> Str { return description; }

    bool FCommandLineParameter::operator<(const FCommandLineParameter *rhs) {
        return fullCLName < rhs->fullCLName;
    }

    void FCommandLineParameter::setDescription(Str newDescription) {
        this->description = newDescription;
    }

    bool FCommandLineParameter::operator==(Str str) const {
        return Common::SplitString(fullCLName, ",")[0] == str;
    }


    OStream &operator<<(OStream &out, const FCommandLineParameter &b) {
        out << b.ValueToString();
        return out;
    }

    OStream &operator<<(OStream &out, const FCommandLineParameter *b) {
        out << b->ValueToString();
        return out;
    }

}