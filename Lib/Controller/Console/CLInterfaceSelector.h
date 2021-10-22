//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_COMMANDLINEINPUTMANAGER_H
#define V_SHAPE_COMMANDLINEINPUTMANAGER_H

#include "Apps/Simulations/BCInterface.h"

class CLInterfaceSelector : Interface {
private:
    CLInterfaceSelector();
    static CLInterfaceSelector *mySingleInstance;

    int currentSelection = 0;

    IntegerParameter selection{0, "sim", "Sim type selection"};
public:
    static auto getInstance() -> CLInterfaceSelector &;

    void registerBCInterface(Base::BCInterface *bcInterface);

    void setup(int argc, const char **argv);
    auto getSelectedBCInterface() const -> Base::BCInterface *;

private:
    std::vector<Base::BCInterface*> bcInterfaces;
};


#endif //V_SHAPE_COMMANDLINEINPUTMANAGER_H
