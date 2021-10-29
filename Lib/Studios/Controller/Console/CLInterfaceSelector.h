//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "../Interface/Interface.h"
#include "../Interface/CommonParameters.h"

class CLInterfaceSelector : Interface {
private:
    CLInterfaceSelector();
    static CLInterfaceSelector *mySingleInstance;

    int currentSelection = 0;

    IntegerParameter selection{0, "sim", "Sim type selection"};
public:
    static auto getInstance() -> CLInterfaceSelector &;

    void registerBCInterface(Interface *interface);

    void setup(int argc, const char **argv);
    auto getCurrentSelectedInterface() const -> Interface *;

private:
    std::vector<Interface*> bcInterfaces;
};


#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
