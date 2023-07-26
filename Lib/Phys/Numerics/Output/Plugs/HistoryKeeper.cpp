#include "HistoryKeeper.h"

#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Common/Log/Log.h"
#include "Mappings/R2toR/Model/EquationState.h"

#include <Base/Controller/Interface/InterfaceManager.h>

const long long unsigned int ONE_GB = 1073741824;

HistoryKeeper::HistoryKeeper(const NumericParams &params, size_t recordStepsInterval, SpaceFilterBase *filter)
    : Numerics::OutputSystem::Socket(params, "History output", int(recordStepsInterval)), spaceFilter(*filter),
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
    return count * Numerics::Allocator::GetInstance().getNumericParams().getN() * sizeof(Real);
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

    auto data = spaceFilter(outInfo);
    spaceDataHistory.emplace_back(data);
    tHistory.push_back(outInfo.getSimTime());

    #if false
    {
        const auto eps = 0.05;
        const DiscreteSpace &phiSpace = *outInfo.getSpaceData().first;
        R2toR::DiscreteFunction &f = outInfo.getEqStateData<R2toR::EquationState>()->getPhi();
        const R2toR::Domain domain = f.getDomain();
        const auto L = domain.getLx();
        auto is = outInfo.getSimTime() > L / 2 - eps && outInfo.getSimTime() < L / 2 + eps;
        if (is) {
            Log::Debug("@t=") << outInfo.getSimTime() << Log::Flush;

            auto N_ = 32;
            auto M_ = 2*N_/3;
            auto dx = L / N_;
            auto dy = L / M_;

            auto c = 0;
            for (Real y = -L / 2; y < L / 2; y += dy) {
                std::cout << std::endl << ++c << (c < 10 ? "   " : "  ");
                for (Real x = -L / 2; x < L / 2; x += dx) {
                    {
                        Real2D r = {x, y};
                        auto val = Common::GetDensityChar(f(r));
                        std::cout << val << " ";
                    }

                }
            }
        }
    }
    #endif


    ++count;
}

auto HistoryKeeper::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    _dump(true);
    return true;
}

auto HistoryKeeper::renderMetaDataAsPythonDictionary() const -> Str {
    Numerics::Allocator &builder = Numerics::Allocator::GetInstance();

    std::ostringstream oss;

    oss << R"({, "outresT": " << (countTotal+count))";

    DimensionMetaData recDim = spaceFilter.getOutputDim();
    Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];
    oss << R"(, "data_channels": 2)";
    oss << R"str(, "data_channel_names": ("phi", "ddtphi"), )str";

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEntries();
    oss << "}" << std::endl;

    return oss.str();
}
