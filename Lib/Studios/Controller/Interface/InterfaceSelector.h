//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "Interface.h"
#include "CommonParameters.h"

class InterfaceSelector : Interface {

    InterfaceSelector();
    static InterfaceSelector *mySingleInstance;

    int currentSelection = 0;
    std::vector<Interface*> candidates;

    IntegerParameter selection{0, "sim", "Sim type selection"};

public:
    static auto getInstance() -> InterfaceSelector &;

    void registerCandidate(Interface *interface);

    void setup(int argc, const char **argv);
    auto getCurrentCandidate() const -> Interface *;

};


#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
