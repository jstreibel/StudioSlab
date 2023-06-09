//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "Interface.h"
#include "CommonParameters.h"

class InterfaceSelector : Interface {
    static InterfaceSelector *mySingleInstance;

    int currentSelection = 0;
    std::vector<Interface::Ptr> candidates;

    IntegerParameter::Ptr selection = std::make_shared<IntegerParameter>(0, "sim", "Sim type selection");

public:
    InterfaceSelector(String selectorName);

    /**
     * This is left here for compatibility reasons.
     * @return
     */
    static auto getInstance() -> InterfaceSelector &;

    void registerOption(Interface::Ptr interface);

    auto preParse(int argc, const char **argv) -> const InterfaceSelector&;
    auto getCurrentCandidate() const -> Interface::Ptr;

};


#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
