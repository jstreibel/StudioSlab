//
// Created by joao on 09/09/2019.
//

#include "InterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "Base/Controller/CLArgsManager.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Utils.h"


InterfaceSelector::InterfaceSelector(String name) : Interface(name)
{
    addParameters({selection});
};


InterfaceSelector *InterfaceSelector::mySingleInstance = nullptr;


auto InterfaceSelector::getInstance() -> InterfaceSelector & {
    if (mySingleInstance == nullptr) {
        mySingleInstance = new InterfaceSelector("Available boundary conditions");
        std::cout << "WARNING: InterfaceSelector is no longer to be used as singleton! This feature is "
                     "deprecated and will be removed very soon." << std::endl;
    }

    return *mySingleInstance;
}

auto InterfaceSelector::getCurrentCandidate() const -> Interface::Ptr {
    if(currentSelection > candidates.size() - 1)
        throw String("Unknown sim type: ") + ToString(currentSelection);

    return candidates[currentSelection];
}

auto InterfaceSelector::preParse(int argc, const char **argv) -> const InterfaceSelector& {
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

    auto sim = getCurrentCandidate();

    auto &interfaceManager = InterfaceManager::getInstance();

    interfaceManager.registerInterface(sim);

    auto subInterfaces = sim->getSubInterfaces();
    for(auto subInterface : subInterfaces)
        interfaceManager.registerInterface(subInterface);

    return *this;
}

void InterfaceSelector::registerOption(Interface::Ptr interface) {
    candidates.push_back(interface);

    StringStream simsHelp;
    simsHelp << "Sim types:\n";

    for(int i=0; i < candidates.size(); i++)
        simsHelp << i << ". " << candidates[i]->getGeneralDescription() << "\n";

    selection->setDescription(simsHelp.str());
}

