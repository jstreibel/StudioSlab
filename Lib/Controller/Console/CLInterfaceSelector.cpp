//
// Created by joao on 09/09/2019.
//

#include "CLInterfaceSelector.h"

#include "Apps/Simulations/BCInterface.h"
#include <boost/algorithm/string.hpp>


#include "Core/Util/Workaround/StringStream.h"
#include "CLInterfaceManager.h"

#include "Controller/Interface/InterfaceManager.h"


CLInterfaceSelector::CLInterfaceSelector() : Interface("Available boundary conditions")
{
    addParameters({&selection});
};


CLInterfaceSelector *CLInterfaceSelector::mySingleInstance = nullptr;


auto CLInterfaceSelector::getInstance() -> CLInterfaceSelector & {
    if (mySingleInstance == nullptr) mySingleInstance = new CLInterfaceSelector;

    return *mySingleInstance;
}

auto CLInterfaceSelector::getSelectedBCInterface() const -> Base::BCInterface * {
    const int simType = currentSelection;

    CLInterfaceSelector &me = CLInterfaceSelector::getInstance();
    if(simType > me.bcInterfaces.size()-1) throw (String("Unknown sim type: ") + std::to_string(simType)).c_str();

    return me.bcInterfaces[simType];
}

void CLInterfaceSelector::setup(int argc, const char **argv) {
    typedef std::string str;

    const str simStr("--sim");
    for(int i=0; i<argc; i++){
        const str arg(argv[i]);
        if(boost::contains(arg, simStr))
        {
            std::vector<str> result;
            boost::split(result, arg, [](char c){return (c == '=');});

            if (result.size() == 1) {
                if (i < argc - 1) currentSelection = std::stoi(argv[i + 1]);
            }
            else if (result.size() == 2){
                currentSelection = stoi(result[1]);
            }
            else {
                throw "Command line error.";
            }
        }
    }

    auto *sim = getSelectedBCInterface();

    auto &interfaceManager = InterfaceManager::getInstance();
    interfaceManager.registerInterface(sim);
    interfaceManager.registerInterface(sim->getOutputStructureBuilder());

}

void CLInterfaceSelector::registerBCInterface(Base::BCInterface *bcInterface) {
    bcInterfaces.push_back(bcInterface);

    //selection.

    StringStream simsHelp;
    simsHelp << "Sim types:\n";

    for(int i=0; i<bcInterfaces.size(); i++)
        simsHelp << i << ". " << bcInterfaces[i]->getGeneralDescription() << "\n";

    selection.setDescription(simsHelp.str());
}

