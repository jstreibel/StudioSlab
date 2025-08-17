//
// Created by joao on 10/13/21.
//

#include "Parameter.h"

#include "CommandLine/CommandLineHelpers.h"
#include "Utils/StringFormatting.h"
#include "Utils/Utils.h"


namespace Slab::Core {
    FParameter::FParameter(const FParameterDescription& Description)
    : Description(Description)
    {

    }

    Str FParameter::GetName() const
    {
        return Description.Name;
    }

    auto FParameter::GetFullCommandLineName() const -> Str {
        if (Description.Rubric == '\0') return SanitizeToValidLongOption(Description.Name, Description.Formatting);

        return Str(1, Description.Rubric) + "," + SanitizeToValidLongOption(Description.Name, Description.Formatting);
    }

    auto FParameter::GetCommandLineArgumentName(bool LongNameIfPresent) const -> Str {
        if (LongNameIfPresent) return SanitizeToValidLongOption(Description.Name, Description.Formatting);

        return Description.Rubric != '\0' ? Str(1, Description.Rubric) : SanitizeToValidLongOption(Description.Name, Description.Formatting);
    }

    auto FParameter::GetDescription() const -> Str { return Description.Description; }

    bool FParameter::operator<(const FParameter *rhs) const
    {
        return Description.Name < rhs->Description.Name;
    }

    void FParameter::SetDescription(const Str& NewDescription) {
        Description.Description = NewDescription;
    }

    bool FParameter::operator==(Str str) const {
        return Description.Name == str;
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