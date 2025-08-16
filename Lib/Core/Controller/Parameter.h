//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "CommandLine/CommandLineParserDefs.h"

#include "Utils/Types.h"

namespace Slab::Core {

    class FParameter {
    protected:
        Str shortCLName, longCLName, fullCLName, description;

    public:
        virtual ~FParameter() = default;
        FParameter(const Str &commandLineArgName, const Str &description);

        virtual auto AddToCommandLineOptionsGroup(CLODEasyInit &group) const -> void = 0;
        auto getFullCommandLineName() const -> Str;
        auto getCommandLineArgumentName(bool longNameIfPresent = false) const -> Str;

        virtual auto ValueToString() const -> Str = 0;

        virtual void SetValueFromCommandLine(VariableValue var) = 0;

        virtual void SetValue(const void *) = 0;

        virtual auto GetValueVoid() const -> const void * = 0;

        template<typename T>
        const T& getValueAs() const { return *static_cast<const T*>(GetValueVoid()); }

        auto getDescription() const -> Str;

        void setDescription(Str newDescription);

        bool operator<(const FParameter *rhs);

        bool operator==(Str str) const;

    };

    DefinePointers(FParameter)

    OStream &operator<<(OStream &out, const FParameter &b);

    OStream &operator<<(OStream &out, const FParameter *b);


}

#endif //FIELDS_PARAMETERBASE_H
