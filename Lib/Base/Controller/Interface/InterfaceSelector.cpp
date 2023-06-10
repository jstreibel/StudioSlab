//
// Created by joao on 09/09/2019.
//

#include "InterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "Base/Controller/CLArgsManager.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Utils.h"
#include "Common/Log/Log.h"


InterfaceSelector::InterfaceSelector(String name) : InterfaceOwner(name, -1, true)
{
    interface->addParameters({selection});
};


InterfaceSelector *InterfaceSelector::mySingleInstance = nullptr;


auto InterfaceSelector::getInstance() -> InterfaceSelector & {
    if (mySingleInstance == nullptr) {
        mySingleInstance = new InterfaceSelector("Available boundary conditions");
        Log::Warning() << "InterfaceSelector is no longer to be used as singleton! This feature is "
                     "deprecated and will be removed very soon." << Log::Flush;
    }

    return *mySingleInstance;
}

auto InterfaceSelector::getCurrentCandidate() const -> Interface::Ptr {
    if(currentSelection > candidates.size() - 1)
        throw String("Unknown sim type: ") + ToString(currentSelection);

    return candidates[currentSelection];
}

auto InterfaceSelector::preParse(int argc, const char **argv, bool registerInInterfaceManager) -> const InterfaceSelector& {
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

    if(registerInInterfaceManager) {
        auto selection = getCurrentCandidate();

        auto &interfaceManager = InterfaceManager::getInstance();

        interfaceManager.registerInterface(selection);

        auto subInterfaces = selection->getSubInterfaces();
        for (auto subInterface: subInterfaces)
            interfaceManager.registerInterface(subInterface);
    }

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

