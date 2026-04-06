#include "HistoryKeeper.h"

#include "Math/Numerics/OutputChannel.h"
#include "Core/Tools/Log.h"

#include "../../../../../Core/Controller/InterfaceManager.h"


namespace Slab::Math {

    const long long unsigned int ONE_GB = 1073741824;

    FHistoryKeeper::FHistoryKeeper(size_t recordStepsInterval, FSpaceFilterBase *filter)
            : FOutputChannel("History output", static_cast<int>(recordStepsInterval)), spaceFilter(*filter),
              count(0), countTotal(0) {
        // TODO: assert(ModelBuilder::getInstance().getParams().getN()>=outputResolutionX);
    }

    FHistoryKeeper::~FHistoryKeeper() {
        delete &spaceFilter;
    }

    auto FHistoryKeeper::getUtilMemLoadBytes() const -> long long unsigned int {
        // TODO fazer esse calculo baseado no tamanho de cada instante de tempo do campo, e contemplando o modelo de fato
        //  em que estamos trabalhando (1d, 2d, escalar, SU(2), etc.).
        //  Em outras palavras: o calculo abaixo esta errado.

        if(spaceDataHistory.empty()) return 0;

        IN reference = spaceDataHistory.front().first;
        fix N = reference->getTotalDiscreteSites();

        return count * N * sizeof(DevFloat);
    }

    auto FHistoryKeeper::ShouldOutput(long unsigned timestep) -> bool {
        // const bool should = (/*t >= tStart && */t <= tEnd) && Socket::shouldOutput(t, timestep);

        // return should;

        return FOutputChannel::ShouldOutput(timestep);;
    }

    void FHistoryKeeper::HandleOutput(const FOutputPacket &packet) {
        if (getUtilMemLoadBytes() > 4 * ONE_GB) {
            Core::FLog::Critical() << "Dumping " << (getUtilMemLoadBytes() * 4e-6) << "GB of data." << Core::FLog::Flush;
            this->_dump(false);
            countTotal += count;
            count = 0;
            spaceDataHistory.clear(); // TODO compute total liberated memory from this history
            Core::FLog::Success() << "Memory dump successful." << Core::FLog::Flush;
        }

        spaceDataHistory.emplace_back(spaceFilter(packet));
        stepHistory.push_back(packet.GetSteps());

        ++count;
    }

    auto FHistoryKeeper::NotifyIntegrationHasFinished(const FOutputPacket &theVeryLastOutputInformation) -> bool {
        _dump(true);
        return true;
    }

    auto FHistoryKeeper::renderMetaDataAsPythonDictionary() const -> Str {
        std::ostringstream oss;

        oss << R"({, "outresT": " << (countTotal+count))";

        DimensionMetaData recDim = spaceFilter.getOutputDim();
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);
        oss << R"(, "data_channels": 2)";
        oss << R"str(, "data_channel_names": ("phi", "ddtphi"), )str";

        oss << ", " << Core::FInterfaceManager::GetInstance().RenderAsPythonDictionaryEntries();
        oss << "}" << std::endl;

        return oss.str();
    }


}
