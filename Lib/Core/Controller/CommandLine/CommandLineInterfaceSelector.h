//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "CommandLineInterface.h"
#include "CommandLineInterfaceOwner.h"
#include "CommandLineCommonParameters.h"


namespace Slab::Core {

    class CLInterfaceSelector : public FCommandLineInterfaceOwner {
        int currentSelection = 0;
        Vector<Pointer<FCommandLineInterface>> candidates;

        IntegerParameter selection = IntegerParameter(0, "sim", "Sim type selection");

        auto generateHelpDescription() -> void;

    public:
        CLInterfaceSelector(Str selectorName);

        void registerOption(Pointer<FCommandLineInterface> interface);

        auto preParse(int argc, const char **argv, bool registerInInterfaceManager = true) -> const CLInterfaceSelector &;

        auto getCurrentCandidate() const -> Pointer<FCommandLineInterface>;

    };


}

#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
