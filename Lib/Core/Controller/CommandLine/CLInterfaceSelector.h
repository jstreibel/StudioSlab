//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "CLInterface.h"
#include "CLInterfaceOwner.h"
#include "Core/Controller/Parameter/CommonParameters.h"


namespace Slab::Core {

    class CLInterfaceSelector : public CLInterfaceOwner {
        int currentSelection = 0;
        Vector<CLInterface_ptr> candidates;

        IntegerParameter selection = IntegerParameter(0, "sim", "Sim type selection");

        auto generateHelpDescription() -> void;

    public:
        CLInterfaceSelector(Str selectorName);

        void registerOption(CLInterface_ptr interface);

        auto preParse(int argc, const char **argv, bool registerInInterfaceManager = true) -> const CLInterfaceSelector &;

        auto getCurrentCandidate() const -> CLInterface_ptr;

    };


}

#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
