#include "OutputHistory.h"

#include <Studios/Controller/Interface/InterfaceManager.h>
#include <Fields/Core/Allocator.h>

const long long unsigned int ONE_GB = 1073741824;

OutputHistory::OutputHistory(size_t recordStepsInterval, SpaceFilterBase *filter, double tEnd_)
    : OutputChannel(int(recordStepsInterval)), spaceFilter(*filter), tEnd(tEnd_),
      count(0), countTotal(0)
{
    // TODO: assert(ModelBuilder::getInstance().getParams().getN()>=outputResolutionX);
}

OutputHistory::~OutputHistory() {
    delete &spaceFilter;
}

auto OutputHistory::getUtilMemLoadBytes() const -> long long unsigned int
{
    // TODO fazer esse calculo baseado no tamanho de cada instante de tempo do campo, e contemplando o modelo de fato
    //  em que estamos trabalhando (1d, 2d, escalar, SU(2), etc.).
    //  Em outras palavras: o calculo abaixo esta errado.
    return count * Allocator::getInstance().getNumericParams().getN() * sizeof(double);
}

auto OutputHistory::shouldOutput(double t, long unsigned timestep) -> bool{
    const bool should = (/*t >= tStart && */t <= tEnd) && OutputChannel::shouldOutput(t, timestep);

    return should;
}

void OutputHistory::_out(const OutputPacket &outInfo)
{
    if(getUtilMemLoadBytes() > 4*ONE_GB){
        std::cout << "\n\nDumping mem... " << std::flush;
        this->_dump(false);
        countTotal += count;
        count = 0;
        spaceDataHistory.clear(); // TODO compute total liberated memory from this history
        std::cout << "Done!" << std::endl;
    }

    spaceDataHistory.emplace_back(spaceFilter(outInfo));
    tHistory.push_back(outInfo.getT());

    ++count;
}

auto OutputHistory::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    _dump(true);
    return true;
}

auto OutputHistory::renderMetaDataAsPythonDictionary() const -> String {
    Allocator &builder = Allocator::getInstance();

    std::ostringstream oss;

    oss << R"({, "outresT": " << (countTotal+count))";

    DimensionMetaData recDim = spaceFilter.getOutputDim();
    String dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];
    oss << R"(, "data_channels": 2)";
    oss << R"str(, "data_channel_names": ("phi", "ddtphi"), )str";

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEnrties();
    oss << "}" << std::endl;

    return oss.str();
}

//void OutputHistory::_dump(){ }
