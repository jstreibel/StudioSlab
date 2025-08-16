//
// Created by joao on 09/09/2019.
//

#include "CommandLineInterfaceSelector.h"

#include <boost/algorithm/string.hpp>

#include "../InterfaceManager.h"
#include "Utils/Utils.h"
#include "Core/Tools/Log.h"



namespace Slab::Core {


    FCommandLineInterfaceSelector::FCommandLineInterfaceSelector(Str name) : FInterfaceOwner(name + " interface selector", -1, true) {
        Interface->AddParameters({&selection});
    };

    auto FCommandLineInterfaceSelector::GetCurrentCandidate() const -> TPointer<FInterface> {
        if (CurrentSelection > Candidates.size() - 1)
            throw Str("Unknown sim type: ") + ToStr(CurrentSelection);

        return Candidates[CurrentSelection];
    }

    auto FCommandLineInterfaceSelector::PreParse(int argc, const char **argv,
                                       bool registerInInterfaceManager) -> const FCommandLineInterfaceSelector & {
        typedef std::string str;

        const str simStr("--sim");
        for (int i = 0; i < argc; i++) {
            const str arg(argv[i]);
            if (boost::contains(arg, simStr)) {
                Vector<str> result;
                boost::split(result, arg, [](char c) { return (c == '='); });

                if (result.size() == 1) {
                    if (i < argc - 1) CurrentSelection = std::stoi(argv[i + 1]);
                } else if (result.size() == 2) {
                    CurrentSelection = stoi(result[1]);
                } else {
                    throw "Command line error.";
                }
            }
        }

        auto currSelection = GetCurrentCandidate();
        Log::Critical() << "InterfaceSelector '" << Interface->GetName()
                        << "' has pre-parsed its options. Selection is '"
                        << currSelection->GetName() << "'." << Log::Flush;

        if (registerInInterfaceManager) {
            Log::Info(
                    "InterfaceSelector is registering selected interface (and sub-interfaces if present) in InterfaceManager.");

            auto &interfaceManager = FInterfaceManager::GetInstance();

            interfaceManager.RegisterInterface(currSelection);
        }

        GenerateHelpDescription();

        return *this;
    }

    void FCommandLineInterfaceSelector::RegisterOption(TPointer<FInterface> interface) {
        Candidates.push_back(interface);

        GenerateHelpDescription();
    }

    auto FCommandLineInterfaceSelector::GenerateHelpDescription() -> void {
        StringStream simsHelp;
        simsHelp << "Sim types:\n";

        auto curr = GetCurrentCandidate();
        for (int i = 0; i < Candidates.size(); i++) {
            auto cand = Candidates[i];
            if (*cand == *curr) simsHelp << Log::BoldFace << Log::FGBlue;
            simsHelp << i << ". " << cand->GetName();
            if (*cand == *curr) simsHelp << Log::ResetFormatting;
            simsHelp << "\n";
        }

        selection.SetDescription(simsHelp.str());
    }

}