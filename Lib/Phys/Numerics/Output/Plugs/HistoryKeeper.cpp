#include "HistoryKeeper.h"

#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Common/Log/Log.h"

#include <Base/Controller/Interface/InterfaceManager.h>

const long long unsigned int ONE_GB = 1073741824;

HistoryKeeper::HistoryKeeper(size_t recordStepsInterval, SpaceFilterBase *filter, Real tEnd_)
    : Numerics::OutputSystem::Socket("History output", int(recordStepsInterval)), spaceFilter(*filter), tEnd(tEnd_),
      count(0), countTotal(0)
{
    // TODO: assert(ModelBuilder::getInstance().getParams().getN()>=outputResolutionX);
}

HistoryKeeper::~HistoryKeeper() {
    delete &spaceFilter;
}

auto HistoryKeeper::getUtilMemLoadBytes() const -> long long unsigned int
{
    // TODO fazer esse calculo baseado no tamanho de cada instante de tempo do campo, e contemplando o modelo de fato
    //  em que estamos trabalhando (1d, 2d, escalar, SU(2), etc.).
    //  Em outras palavras: o calculo abaixo esta errado.
    return count * Numerics::Allocator::getInstance().getNumericParams().getN() * sizeof(Real);
}

auto HistoryKeeper::shouldOutput(Real t, long unsigned timestep) -> bool{
    const bool should = (/*t >= tStart && */t <= tEnd) && Numerics::OutputSystem::Socket::shouldOutput(t, timestep);

    return should;
}

void HistoryKeeper::_out(const OutputPacket &outInfo)
{
    if(getUtilMemLoadBytes() > 4*ONE_GB){
        Log::Critical() << "Dumping "<< (getUtilMemLoadBytes()*4e-6) << "GB of data." << Log::Flush;
        this->_dump(false);
        countTotal += count;
        count = 0;
        spaceDataHistory.clear(); // TODO compute total liberated memory from this history
        Log::Success() << "Memory dump successful." << Log::Flush;
    }

    spaceDataHistory.emplace_back(spaceFilter(outInfo));
    tHistory.push_back(outInfo.getSimTime());

    ++count;
}

auto HistoryKeeper::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    _dump(true);
    return true;
}

auto HistoryKeeper::renderMetaDataAsPythonDictionary() const -> String {
    Numerics::Allocator &builder = Numerics::Allocator::getInstance();

    std::ostringstream oss;

    oss << R"({, "outresT": " << (countTotal+count))";

    DimensionMetaData recDim = spaceFilter.getOutputDim();
    String dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];
    oss << R"(, "data_channels": 2)";
    oss << R"str(, "data_channel_names": ("phi", "ddtphi"), )str";

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEntries();
    oss << "}" << std::endl;

    return oss.str();
}
