//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "CommandLine/CommandLineHelpers.h"
#include "CommandLine/CommandLineParserDefs.h"

#include "Utils/Types.h"

namespace Slab::Core {

    struct FParameterDescription {
        FParameterDescription(const Str& Name, const Str& Description, FLongOptionFormatting Formatting = FLongOptionFormatting())
        : FParameterDescription('\0',   Name,          Description, Formatting) {}
        FParameterDescription(const char Rubric, const Str& Description, FLongOptionFormatting Formatting = FLongOptionFormatting())
        : FParameterDescription(Rubric, Str(1, Rubric), Description, Formatting) {}
        FParameterDescription(const char Rubric, const Str& Name, const Str& Description, FLongOptionFormatting Formatting = FLongOptionFormatting())
        : Name(Name), Rubric(Rubric), Description(Description), Formatting(Formatting) {}

        Str Name;
        char Rubric;
        Str Description;

        FLongOptionFormatting Formatting;
    };

    class FParameter {

    public:
        virtual ~FParameter() = default;
        explicit FParameter(const FParameterDescription&);

        Str GetName() const;

        virtual auto AddToCommandLineOptionsGroup(CLODEasyInit &group) const -> void = 0;
        auto GetFullCommandLineName() const -> Str;
        auto GetCommandLineArgumentName(bool longNameIfPresent = false) const -> Str;

        virtual auto ValueToString() const -> Str = 0;

        virtual void SetValueFromCommandLine(VariableValue var) = 0;

        virtual void SetValue(const void *) = 0;

        virtual auto GetValueVoid() const -> const void * = 0;

        template<typename T>
        const T& GetValueAs() const { return *static_cast<const T*>(GetValueVoid()); }

        auto GetDescription() const -> Str;

        void SetDescription(const Str& NewDescription);

        bool operator<(const FParameter *rhs) const;

        bool operator==(Str str) const;

    private:
        FParameterDescription Description;

        // Str shortCLName, longCLName, fullCLName, description;
    };

    DefinePointers(FParameter)

    OStream &operator<<(OStream &out, const FParameter &b);

    OStream &operator<<(OStream &out, const FParameter *b);


}

#endif //FIELDS_PARAMETERBASE_H
