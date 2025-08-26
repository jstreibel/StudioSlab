//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "../Interface.h"
#include "../InterfaceOwner.h"
#include "../Parameter/BuiltinParameters.h"


namespace Slab::Core {

    class FCommandLineInterfaceSelector : public FInterfaceOwner {
        int CurrentSelection = 0;
        Vector<TPointer<FInterface>> Candidates;

        IntegerParameter selection = IntegerParameter(0, {"sim", "Sim type selection"}, {});

        auto GenerateHelpDescription() -> void;

    public:
        explicit FCommandLineInterfaceSelector(Str selectorName);

        void RegisterOption(TPointer<FInterface> interface);

        auto PreParse(int argc, const char **argv, bool registerInInterfaceManager = true) -> const FCommandLineInterfaceSelector &;

        auto GetCurrentCandidate() const -> TPointer<FInterface>;

    };


}

#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
