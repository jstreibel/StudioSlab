//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "Interface.h"
#include "InterfaceOwner.h"
#include "CommonParameters.h"

class InterfaceSelector : public InterfaceOwner {
    int currentSelection = 0;
    std::vector<Interface::Ptr> candidates;

    IntegerParameter selection = IntegerParameter(0, "sim", "Sim type selection");

    auto generateHelpDescription() -> void;
public:
    InterfaceSelector(Str selectorName);

    void registerOption(Interface::Ptr interface);

    auto preParse(int argc, const char **argv, bool registerInInterfaceManager=true) -> const InterfaceSelector&;
    auto getCurrentCandidate() const -> Interface::Ptr;

};


#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
