//
// Created by joao on 20/09/23.
//

#include "SnapshotOutput.h"

#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Tools/Log.h"

#include "../KG-RtoREquationState.h"


namespace Slab::Models::KGRtoR {

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


    auto SnapshotOutput::filterData(const OutputPacket &packet) -> RtoR::NumericFunction_CPU {
        NOT_IMPLEMENTED
        /*
        auto &phi = packet.GetNakedStateData<EquationState>()->getPhi();

        return RtoR::NumericFunction_CPU(phi);
         */
    }


    bool SnapshotOutput::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) {
        Socket::notifyIntegrationHasFinished(theVeryLastOutputInformation);

        auto f = filterData(theVeryLastOutputInformation);

        return OutputNumericFunction(Naked(f), outputFileName);
    }

    bool SnapshotOutput::OutputNumericFunction(const Pointer<RtoR::NumericFunction>& funky,
                                               const Str& outputFileName,
                                               Vector<Pair<Str,Str>> xtraPyDictEntries)
    {
        std::ofstream outputFile(outputFileName, std::ios::out | std::ios::binary);
        if(!outputFile){
            Log::Error() << "Could not open '" << outputFileName << "' for snapshot output." << Log::Flush;
            return false;
        }

        fix SEPARATOR = "\n<<<<-oOo->>>>\n";

        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;
        outputFile << "{" << CLInterfaceManager::getInstance().renderAsPythonDictionaryEntries();
        for(auto entry : xtraPyDictEntries) // {key: value, key:value, }
            outputFile << "\"" << entry.first << "\": " << entry.second << ", ";
        outputFile << "} " << SEPARATOR << std::flush;

        auto &data = funky->getSpace().getHostData(true);
        auto vecData = std::vector<double>(std::begin(data), std::end(data));
        outputFile.write(reinterpret_cast<const char*>(&data[0]), data.size()*sizeof(Real));

        outputFile.close();
        Log::Success() << "Snapshot saved to '" << outputFileName << "'." << Log::Flush;

        return true;
    }


} // KleinGordon::RtoR