#include "HistoryKeeper.h"

#include "Math/Numerics/OutputChannel.h"
#include "Core/Tools/Log.h"

#include "../../../../../Core/Controller/InterfaceManager.h"


namespace Slab::Math {

    const long long unsigned int ONE_GB = 1073741824;

    FHistoryKeeper::FHistoryKeeper(size_t recordStepsInterval, FSpaceFilterBase *filter, size_t maxBufferedBytes)
            : FOutputChannel("History output", static_cast<int>(recordStepsInterval)),
              MaxBufferedBytes(maxBufferedBytes), spaceFilter(*filter), count(0), countTotal(0) {
        // TODO: assert(ModelBuilder::getInstance().getParams().getN()>=outputResolutionX);
    }

    FHistoryKeeper::~FHistoryKeeper() {
        ClearBufferedHistory();
        delete &spaceFilter;
    }

    auto FHistoryKeeper::getUtilMemLoadBytes() const -> long long unsigned int {
        if (spaceDataHistory.empty()) return 0;

        const auto &reference = spaceDataHistory.front();
        const auto phiSites = reference.first != nullptr ? reference.first->getTotalDiscreteSites() : 0;
        const auto dPhiDtSites = reference.second != nullptr ? reference.second->getTotalDiscreteSites() : 0;

        return count * ((phiSites + dPhiDtSites) * sizeof(DevFloat) + sizeof(FRealVector::value_type));
    }

    auto FHistoryKeeper::ShouldOutput(long unsigned timestep) -> bool {
        // const bool should = (/*t >= tStart && */t <= tEnd) && Socket::shouldOutput(t, timestep);

        // return should;

        return FOutputChannel::ShouldOutput(timestep);;
    }

    void FHistoryKeeper::HandleOutput(const FOutputPacket &packet) {
        if (count > 0 && getUtilMemLoadBytes() >= MaxBufferedBytes) {
            Core::FLog::Critical() << "Dumping "
                                   << static_cast<DevFloat>(getUtilMemLoadBytes()) / static_cast<DevFloat>(ONE_GB)
                                   << "GB of data." << Core::FLog::Flush;
            this->_dump(false);
            countTotal += count;
            count = 0;
            ClearBufferedHistory();
            Core::FLog::Success() << "Memory dump successful." << Core::FLog::Flush;
        }

        spaceDataHistory.emplace_back(spaceFilter(packet));
        stepHistory.push_back(packet.GetSteps());

        ++count;
    }

    auto FHistoryKeeper::NotifyIntegrationHasFinished(const FOutputPacket &theVeryLastOutputInformation) -> bool {
        _dump(true);
        countTotal += count;
        count = 0;
        ClearBufferedHistory();
        return true;
    }

    void FHistoryKeeper::ClearBufferedHistory() {
        for (const auto &[phi, dPhiDt]: spaceDataHistory) {
            delete phi;
            if (dPhiDt != phi) delete dPhiDt;
        }

        spaceDataHistory.clear();
        stepHistory.clear();
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
