//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "../Interface.h"
#include "../InterfaceOwner.h"
#include "../CommonParameters.h"


namespace Slab::Core {

    class CLInterfaceSelector : public FInterfaceOwner {
        int currentSelection = 0;
        Vector<TPointer<FInterface>> candidates;

        IntegerParameter selection = IntegerParameter(0, "sim", "Sim type selection");

        auto generateHelpDescription() -> void;

    public:
        CLInterfaceSelector(Str selectorName);

        void RegisterOption(TPointer<FInterface> interface);

        auto PreParse(int argc, const char **argv, bool registerInInterfaceManager = true) -> const CLInterfaceSelector &;

        auto GetCurrentCandidate() const -> TPointer<FInterface>;

    };


}

#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
