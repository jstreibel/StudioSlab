#include "OutputHistoryToFile.h"


#include <Studios/CoreMath/Allocator.h>
#include <Studios/Controller/Interface/InterfaceManager.h>

#include <Studios/Util/Timer.h>

#include <utility>


OutputHistoryToFile::OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter, Real endT,
                                         String outputFileName, OutputFormatterBase *outputFormatter)
                               : OutputHistory(stepsInterval, spaceFilter, endT),
                                 outFileName(std::move(outputFileName)), outputFormatter(*outputFormatter)
{
    auto flags = std::ios::out;

    file.open(outFileName, flags);

    if(!file) throw "Erro: nao abriu arquivo.";

    std::cout << "O resultado sera salvo em \'" << outFileName << "\'. " << std::endl;

    String spaces(HEADER_SIZE_BYTES-1, ' ');

    file << spaces << '\n';

    // TODO "Colocar essas coisas abaixo em algum lugar";
    // Output header:
    // long long int totalTimeSteps = lastOutInfo.getSteps(); // Isso eh o que deveria ser, mas so opde acontecer ao
    // final da simulacao, de forma que temos o numero preciso de timesteps dados. O valor abaixo e uma estimativa.

    //size_t totalTimeSteps = p.getn();
    //double recordInterval = this->getRecDt(); // Nao lembro porque mas eh isso mesmo.
    //long long int outputResolutionT = p.T/recordInterval;
    //double _lastT = p.T; // ta errado mas vai ter que ser isso (devia ser de fato o ultimo T, this->lastT).

    //file << "# N | timesteps | h | dT | L | T | " << simInfoKeys << " | outresX | outresT | initTime " << std::endl;
    //file << p.N << " " << totalTimeSteps << " " << p.h << " " << p.dT << " " << p.L
    //     << " " << _lastT << " " << simInfoVals << " " << outputResolutionX << " " << outputResolutionT
    //     << " " << startT_ << std::endl;
}

OutputHistoryToFile::~OutputHistoryToFile() {
    auto *f = &outputFormatter;
    delete f;
};

void OutputHistoryToFile::_dump(bool integrationIsFinished) {
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

void OutputHistoryToFile::_printHeaderToFile() {
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
