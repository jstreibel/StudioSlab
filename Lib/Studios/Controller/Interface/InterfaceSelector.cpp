//
// Created by joao on 09/09/2019.
//

#include "InterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "Studios/Controller/CLArgsManager.h"

#include "Studios/Controller/Interface/InterfaceManager.h"

#include <Common/Workaround/StringStream.h>


InterfaceSelector::InterfaceSelector() : Interface("Available boundary conditions")
{
    addParameters({&selection});
};


InterfaceSelector *InterfaceSelector::mySingleInstance = nullptr;


auto InterfaceSelector::getInstance() -> InterfaceSelector & {
    if (mySingleInstance == nullptr) mySingleInstance = new InterfaceSelector;

    return *mySingleInstance;
}

auto InterfaceSelector::getCurrentCandidate() const -> Interface * {
    const int simType = currentSelection;

    InterfaceSelector &me = InterfaceSelector::getInstance();
    if(simType > me.candidates.size() - 1) throw (String("Unknown sim type: ") + std::to_string(simType)).c_str();

    return me.candidates[simType];
}

void InterfaceSelector::preParse(int argc, const char **argv) {
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

    auto *sim = getCurrentCandidate();

    auto &interfaceManager = InterfaceManager::getInstance();

    interfaceManager.registerInterface(sim);
    // TODO this class should work with a more general Interface instead of BCInterface

    //throw "This below needs a proper solution";
    //interfaceManager.registerInterface(sim->getOutputStructureBuilder());

    auto subInterfaces = sim->getSubInterfaces();
    for(auto *subInterface : subInterfaces)
        interfaceManager.registerInterface(subInterface);

}

void InterfaceSelector::registerOption(Interface *interface) {
    candidates.push_back(interface);

    StringStream simsHelp;
    simsHelp << "Sim types:\n";

    for(int i=0; i < candidates.size(); i++)
        simsHelp << i << ". " << candidates[i]->getGeneralDescription() << "\n";

    selection.setDescription(simsHelp.str());
}

