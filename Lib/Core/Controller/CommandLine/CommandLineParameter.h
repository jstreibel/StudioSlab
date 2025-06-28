//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "Core/Controller/CommandLine/CommandLineParserDefs.h"

#include "Utils/Types.h"

namespace Slab::Core {

    class FCommandLineParameter {
    protected:
        Str shortCLName, longCLName, fullCLName, description;

    public:
        FCommandLineParameter(const Str &commandLineArgName, const Str &description);

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

        bool operator<(const FCommandLineParameter *rhs);

        bool operator==(Str str) const;

    };

    DefinePointers(FCommandLineParameter)

    OStream &operator<<(OStream &out, const FCommandLineParameter &b);

    OStream &operator<<(OStream &out, const FCommandLineParameter *b);


}

#endif //FIELDS_PARAMETERBASE_H
