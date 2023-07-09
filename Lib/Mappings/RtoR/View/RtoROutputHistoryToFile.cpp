//
// Created by joao on 10/8/21.
//

#include "RtoROutputHistoryToFile.h"

#include "Common/Timer.h"

#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Format/SpaceFilterBase.h"
#include "Phys/Numerics/Output/Format/BinarySOF.h"
#include "Base/Controller/Interface/InterfaceManager.h"
#include "Common/Log/Log.h"

const int HEADER_SIZE_BYTES = 2048;

RtoR::OutputHistoryToFile::OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter, Real endT,
                                               Str  outputFileName)
                                               : HistoryKeeper(stepsInterval, spaceFilter, endT),
                                                 outFileName(std::move(outputFileName)),
                                                 outputFormatter(*(new BinarySOF()))
{
    this->name = "Full (1+1) history output";

    file.open(outFileName, std::ios::out);

    if(!file) throw "OutputHistoryToFile: nao abriu arquivo.";

    Log::Info() << "Sim history will be saved in '" << outFileName << "'. " << Log::Flush;

    file << Str(HEADER_SIZE_BYTES - 1, ' ') << '\n';
}

RtoR::OutputHistoryToFile::~OutputHistoryToFile() {
    auto *f = &outputFormatter;
    delete f;
}

void RtoR::OutputHistoryToFile::_dump(bool integrationIsFinished) {
    if(integrationIsFinished){
        _printHeaderToFile();

        auto shouldNotDump = !lastData.hasValidData();
        if(shouldNotDump) {
            file.close();
            return;
        }
    }

    Timer timer;

    for(size_t Ti=0; Ti<count; Ti++) {
        if(timer.getElTime_sec() > 1) {
            timer.reset();
            Log::Info() << std::setprecision(3) << "Flushing " << (Real)Ti/Real(count)*100.0 << "%" << Log::Flush;
        }

        file << outputFormatter(tHistory[int(Ti)]);

        const auto &fieldPair = spaceDataHistory[Ti];
        const DiscreteSpace &phiOut = *fieldPair.first;
        const DiscreteSpace &ddtPhiOut = *fieldPair.second;

        file << outputFormatter(phiOut);
        file << outputFormatter(ddtPhiOut);
    }

    file.flush();

    Log::Success() << "Flushed " << "100%" << Log::Flush;
}


void RtoR::OutputHistoryToFile::_printHeaderToFile() {
    // Allocator &builder = Allocator::getInstance();


    std::ostringstream oss;

    oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal+count);


    DimensionMetaData recDim = spaceFilter.getOutputDim();
    Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];


    oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
    oss << R"(, "data_channels": 2)";
    oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() << "}" << std::endl;

    const auto &s = oss.str();

    file.seekp(0);
    file.write(s.c_str(), (long)s.size());

}

