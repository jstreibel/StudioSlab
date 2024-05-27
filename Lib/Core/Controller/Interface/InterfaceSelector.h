//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "Interface.h"
#include "InterfaceOwner.h"
#include "Core/Controller/Parameter/CommonParameters.h"


namespace Slab::Core {

    class InterfaceSelector : public InterfaceOwner {
        int currentSelection = 0;
        Vector<Interface_ptr> candidates;

        IntegerParameter selection = IntegerParameter(0, "sim", "Sim type selection");

        auto generateHelpDescription() -> void;

    public:
        InterfaceSelector(Str selectorName);

        void registerOption(Interface_ptr interface);

        auto preParse(int argc, const char **argv, bool registerInInterfaceManager = true) -> const InterfaceSelector &;

        auto getCurrentCandidate() const -> Interface_ptr;

    };


}

#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
