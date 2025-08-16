//
// Created by joao on 20/09/23.
//

#include "SnapshotOutput.h"

#include "../../../../../Core/Controller/CommandLineInterfaceManager.h"

namespace Slab::Models::KGRtoR {

    const Str suffix = ".simsnap";

    SnapshotOutput::SnapshotOutput(const Str &fileName,
                                   const Str &socketName, const Str &description)
    : Socket(socketName, -1, description)
    , outputFileName(fileName + suffix)
    {    }

    SnapshotOutput::SnapshotOutput(const Str &fileName)
    : SnapshotOutput(fileName, "Snapshot output", "outputs the last simulation instant")
    {    }


    void SnapshotOutput::HandleOutput(const OutputPacket &) { /* do nothing */ }


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

        return OutputNumericFunction(f.getSpace(), outputFileName);
    }

    bool SnapshotOutput::OutputNumericFunction(const Math::DiscreteSpace& space,
                                               const Str& outputFileName,
                                               const Vector<Pair<Str,Str>>& xtraPyDictEntries)
    {
        std::ofstream outputFile(outputFileName, std::ios::out | std::ios::binary);
        if(!outputFile){
            Core::Log::Error() << "Could not open '" << outputFileName << "' for snapshot output." << Core::Log::Flush;
            return false;
        }

        fix SEPARATOR = "\n<<<<-oOo->>>>\n";

        UseScientificNotation = false;
        RealToStringDecimalPlaces = 7;
        outputFile << "{" << Core::FCommandLineInterfaceManager::GetInstance().RenderAsPythonDictionaryEntries();
        for(const auto&[key, value] : xtraPyDictEntries) // {key: value, key:value, }
            outputFile << "\"" << key << "\": " << value << ", ";
        outputFile.seekp(-2, std::ios_base::cur);
        outputFile << "}" << SEPARATOR << std::flush;

        auto &data = space.getHostData(true);
        auto vecData = std::vector(std::begin(data), std::end(data));
        outputFile.write(reinterpret_cast<const char*>(&data[0]), (long)(data.size()*sizeof(DevFloat)));

        outputFile.close();
        Core::Log::Success() << "Snapshot saved to '" << outputFileName << "'." << Core::Log::Flush;

        return true;
    }


} // KleinGordon::RtoR