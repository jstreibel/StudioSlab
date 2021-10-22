//
// Created by joao on 10/8/21.
//

#include <Controller/Interface/InterfaceManager.h>
#include "RtoROutputHistoryToFile.h"


RtoR::OutputHistoryToFile::OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter, Real endT,
                                               String  outputFileName)
                                               : OutputHistory(stepsInterval, spaceFilter, endT),
                                                 outFileName(std::move(outputFileName)),
                                                 outputFormatter(*(new BinarySOF()))
{
    file.open(outFileName, std::ios::out);

    if(!file) throw "Erro: nao abriu arquivo.";

    std::cout << "O resultado sera salvo em \'" << outFileName << "\'. " << std::endl;

    file << String(HEADER_SIZE_BYTES-1, ' ') << '\n';
}

RtoR::OutputHistoryToFile::~OutputHistoryToFile() {
    auto *f = &outputFormatter;
    delete f;
}

void RtoR::OutputHistoryToFile::_dump(bool integrationIsFinished) {
    if(integrationIsFinished){
        _printHeaderToFile();

        auto shouldNotDump = !lastInfo.hasValidData();
        if(shouldNotDump) {
            file.close();
            return;
        }
    }

    Timer timer;
    timer.resume();

    std::cout << std::setprecision(3);
    std::cout << std::endl;

    for(size_t Ti=0; Ti<count; Ti++) {
        if(timer.getElTime() > 1) {
            timer.reset();
            std::cout << "\rFlushing " << (double)Ti/double(count)*100.0 << "%    " << std::flush;
        }

        file << outputFormatter(tHistory[int(Ti)]);

        const auto &fieldPair = spaceDataHistory[Ti];
        const DiscreteSpace &phiOut = *fieldPair.first;
        const DiscreteSpace &ddtPhiOut = *fieldPair.second;

        file << outputFormatter(phiOut);
        file << outputFormatter(ddtPhiOut);
    }

    file.flush();

    std::cout << "\rFlushing " << "100%    " << std::endl;
}


void RtoR::OutputHistoryToFile::_printHeaderToFile() {
    Allocator &builder = Allocator::getInstance();


    std::ostringstream oss;

    oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal+count);


    DimensionMetaData recDim = spaceFilter.getOutputDim();
    String dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];


    oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
    oss << R"(, "data_channels": 2)";
    oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEnrties() << "}" << std::endl;

    const auto &s = oss.str();

    file.seekp(0);
    file.write(s.c_str(), (long)s.size());

}

