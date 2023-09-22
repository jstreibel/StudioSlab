//
// Created by joao on 20/09/23.
//

#include "SnapshotOutput.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"

#include "../KG-RtoREquationState.h"


namespace KleinGordon::RtoR {

    const Str suffix = ".simsnap";

    SnapshotOutput::SnapshotOutput(const NumericConfig &config, const Str &fileName,
                                   const Str &socketName, const Str &description)
    : Socket(config, socketName, -1, description)
    , outputFileName(fileName + suffix)
    {    }

    SnapshotOutput::SnapshotOutput(const NumericConfig &config, const Str &fileName)
    : SnapshotOutput(config, fileName, "Snapshot output", "outputs the last simulation instant")
    {    }


    void SnapshotOutput::handleOutput(const OutputPacket &) { /* do nothing */ }


    auto SnapshotOutput::filterData(const OutputPacket &packet) -> ::RtoR::DiscreteFunction_CPU {
        auto &phi = packet.getEqStateData<::RtoR::EquationState>()->getPhi();

        return ::RtoR::DiscreteFunction_CPU(phi);
    }


    bool SnapshotOutput::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);

        std::ofstream outputFile(outputFileName, std::ios::out | std::ios::binary);
        if(!outputFile){
            Log::Error() << "Could not open '" << outputFileName << "' for snapshot output." << Log::Flush;
            return false;
        }

        fix SEPARATOR = "\n<<<<-oOo->>>>\n";

        outputFile << "{" << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() << "}" << "   " << SEPARATOR;

        auto f = filterData(theVeryLastOutputInformation);
        auto &data = f.getSpace().getHostData(true);
        outputFile.write(reinterpret_cast<const char*>(&data[0]), (std::streamsize)data.size());

        outputFile.close();
        Log::Success() << "Last moment snapshot saved to '" << outputFileName << "'." << Log::Flush;

        return true;
    }


} // KleinGordon::RtoR