//
// Created by joao on 10/13/21.
//

#include "Parameter.h"

#include "Utils/StringFormatting.h"
#include "Utils/Utils.h"


namespace Slab::Core {

    FParameter::FParameter(FParameterDescription Description)
            : Description(Description.Description)
    {
        Rubric = Description.Rubric;
        Name = Description.Name;
    }

    auto FParameter::GetFullCommandLineName() const -> Str {
        if (Rubric == '\0') return ToSnakeCase(Name);

        return ToStr(Rubric) + "," + ToSnakeCase(Name);
    }

    auto FParameter::GetCommandLineArgumentName(bool longNameIfPresent) const -> Str {
        return ToSnakeCase(longNameIfPresent ? Name : ToStr(Rubric));
    }

    auto FParameter::GetDescription() const -> Str { return Description; }

    bool FParameter::operator<(const FParameter *rhs) const
    {
        return Name < rhs->Name;
    }

    void FParameter::SetDescription(const Str& NewDescription) {
        Description = NewDescription;
    }

    bool FParameter::operator==(Str str) const {
        return Name == str;
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