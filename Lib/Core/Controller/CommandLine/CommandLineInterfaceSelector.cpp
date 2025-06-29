//
// Created by joao on 09/09/2019.
//

#include "CommandLineInterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "CommandLineInterfaceManager.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"



namespace Slab::Core {


    CLInterfaceSelector::CLInterfaceSelector(Str name) : FCommandLineInterfaceOwner(name + " interface selector", -1, true) {
        Interface->AddParameters({&selection});
    };

    auto CLInterfaceSelector::getCurrentCandidate() const -> Pointer<FCommandLineInterface> {
        if (currentSelection > candidates.size() - 1)
            throw Str("Unknown sim type: ") + ToStr(currentSelection);

        return candidates[currentSelection];
    }

    auto CLInterfaceSelector::preParse(int argc, const char **argv,
                                       bool registerInInterfaceManager) -> const CLInterfaceSelector & {
        typedef std::string str;

        const str simStr("--sim");
        for (int i = 0; i < argc; i++) {
            const str arg(argv[i]);
            if (boost::contains(arg, simStr)) {
                Vector<str> result;
                boost::split(result, arg, [](char c) { return (c == '='); });

                if (result.size() == 1) {
                    if (i < argc - 1) currentSelection = std::stoi(argv[i + 1]);
                } else if (result.size() == 2) {
                    currentSelection = stoi(result[1]);
                } else {
                    throw "Command line error.";
                }
            }
        }

        auto currSelection = getCurrentCandidate();
        Log::Critical() << "InterfaceSelector '" << Interface->GetName()
                        << "' has pre-parsed its options. Selection is '"
                        << currSelection->GetName() << "'." << Log::Flush;

        if (registerInInterfaceManager) {
            Log::Info(
                    "InterfaceSelector is registering selected interface (and sub-interfaces if present) in InterfaceManager.");

            auto &interfaceManager = FCommandLineInterfaceManager::getInstance();

            interfaceManager.registerInterface(currSelection);
        }

        generateHelpDescription();

        return *this;
    }

    void CLInterfaceSelector::registerOption(Pointer<FCommandLineInterface> interface) {
        candidates.push_back(interface);

        generateHelpDescription();
    }

    auto CLInterfaceSelector::generateHelpDescription() -> void {
        StringStream simsHelp;
        simsHelp << "Sim types:\n";

        auto curr = getCurrentCandidate();
        for (int i = 0; i < candidates.size(); i++) {
            auto cand = candidates[i];
            if (*cand == *curr) simsHelp << Log::BoldFace << Log::FGBlue;
            simsHelp << i << ". " << cand->GetName();
            if (*cand == *curr) simsHelp << Log::ResetFormatting;
            simsHelp << "\n";
        }

        selection.setDescription(simsHelp.str());
    }

}