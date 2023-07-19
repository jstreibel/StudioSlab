//
// Created by joao on 09/09/2019.
//

#include "InterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "Base/Controller/CLArgsManager.h"

#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Utils.h"
#include "Common/Log/Log.h"


InterfaceSelector::InterfaceSelector(Str name) : InterfaceOwner(name, -1, true)
{
    interface->addParameters({selection});
};

auto InterfaceSelector::getCurrentCandidate() const -> Interface::Ptr {
    if(currentSelection > candidates.size() - 1)
        throw Str("Unknown sim type: ") + ToStr(currentSelection);

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

    auto currSelection = getCurrentCandidate();
    Log::Info() << "InterfaceSelector '" << interface->getName() << "' has pre-parsed its options. Selection is '"
                << currSelection->getName() << "'." << Log::Flush;

    if(registerInInterfaceManager) {
        Log::Note("InterfaceSelector is registering selected interface (and sub-interfaces if present) in InterfaceManager.");

        auto &interfaceManager = InterfaceManager::getInstance();

        interfaceManager.registerInterface(currSelection);
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

