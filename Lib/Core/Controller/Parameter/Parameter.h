//
// Created by joao on 10/13/21.
//

#ifndef FIELDS_PARAMETERBASE_H
#define FIELDS_PARAMETERBASE_H

#include "Core/Controller/CommandLine/CLDefs.h"

#include "Utils/Types.h"

namespace Slab::Core {

    class Parameter {
    protected:
        Str shortCLName, longCLName, fullCLName, description;

    public:
        Parameter(const Str &commandLineArgName, const Str &description);

        virtual auto addToCommandLineOptionsGroup(CLODEasyInit &group) const -> void = 0;
        auto getFullCommandLineName() const -> Str;
        auto getCommandLineArgumentName(bool longNameIfPresent = false) const -> Str;

        virtual auto valueToString() const -> Str = 0;

        virtual void setValueFromCommandLine(VariableValue var) = 0;

        virtual void setValue(const void *) = 0;

        virtual auto getValueVoid() const -> const void * = 0;

        template<typename T>
        const T& getValueAs() const { return *static_cast<const T*>(getValueVoid()); }

        auto getDescription() const -> Str;

        void setDescription(Str newDescription);

        bool operator<(const Parameter *rhs);

        bool operator==(Str str) const;

    };

    DefinePointers(Parameter)

    OStream &operator<<(OStream &out, const Parameter &b);

    OStream &operator<<(OStream &out, const Parameter *b);


}

#endif //FIELDS_PARAMETERBASE_H
