#include "HistoryKeeper.h"

#include "Math/Numerics/Socket.h"
#include "Core/Tools/Log.h"

#include "Core/Controller/CommandLine/CLInterfaceManager.h"


namespace Slab::Math {

    const long long unsigned int ONE_GB = 1073741824;

    HistoryKeeper::HistoryKeeper(size_t recordStepsInterval, SpaceFilterBase *filter)
            : Socket("History output", int(recordStepsInterval)), spaceFilter(*filter),
              count(0), countTotal(0) {
        // TODO: assert(ModelBuilder::getInstance().getParams().getN()>=outputResolutionX);
    }

    HistoryKeeper::~HistoryKeeper() {
        delete &spaceFilter;
    }

    auto HistoryKeeper::getUtilMemLoadBytes() const -> long long unsigned int {
        // TODO fazer esse calculo baseado no tamanho de cada instante de tempo do campo, e contemplando o modelo de fato
        //  em que estamos trabalhando (1d, 2d, escalar, SU(2), etc.).
        //  Em outras palavras: o calculo abaixo esta errado.

        if(spaceDataHistory.empty()) return 0;

        IN reference = spaceDataHistory.front().second;
        fix N = reference->getTotalDiscreteSites();

        return count * N * sizeof(Real);
    }

    auto HistoryKeeper::shouldOutput(long unsigned timestep) -> bool {
        // const bool should = (/*t >= tStart && */t <= tEnd) && Socket::shouldOutput(t, timestep);

        // return should;

        return Socket::shouldOutput(timestep);;
    }

    void HistoryKeeper::handleOutput(const OutputPacket &packet) {
        if (getUtilMemLoadBytes() > 4 * ONE_GB) {
            Core::Log::Critical() << "Dumping " << (getUtilMemLoadBytes() * 4e-6) << "GB of data." << Core::Log::Flush;
            this->_dump(false);
            countTotal += count;
            count = 0;
            spaceDataHistory.clear(); // TODO compute total liberated memory from this history
            Core::Log::Success() << "Memory dump successful." << Core::Log::Flush;
        }

        spaceDataHistory.emplace_back(spaceFilter(packet));
        stepHistory.push_back(packet.getSteps());

        ++count;
    }

    auto HistoryKeeper::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
        _dump(true);
        return true;
    }

    auto HistoryKeeper::renderMetaDataAsPythonDictionary() const -> Str {
        std::ostringstream oss;

        oss << R"({, "outresT": " << (countTotal+count))";

        DimensionMetaData recDim = spaceFilter.getOutputDim();
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);
        oss << R"(, "data_channels": 2)";
        oss << R"str(, "data_channel_names": ("phi", "ddtphi"), )str";

        oss << ", " << Core::CLInterfaceManager::getInstance().renderAsPythonDictionaryEntries();
        oss << "}" << std::endl;

        return oss.str();
    }


}