#include "OutputHistoryToFile.h"
#include "Common/Log/Log.h"


#include <Phys/Numerics/Allocator.h>
#include <Base/Controller/Interface/InterfaceManager.h>

#include <Common/Timer.h>

#include <utility>

const Str extension = ".osc";

OutputHistoryToFile::OutputHistoryToFile(const NumericParams &params, PosInt stepsInterval, SpaceFilterBase *spaceFilter,
                                         Str outputFileName, OutputFormatterBase *outputFormatter)
    : HistoryKeeper(params, stepsInterval, spaceFilter),
      outFileName(std::move(outputFileName + extension + (outputFormatter->isBinary()?"b":""))),
      outputFormatter(*outputFormatter)
{
    auto flags = std::ios::out;

    file.open(outFileName, flags);

    if(!file){
        Log::Error() << "OutputHistoryToFile couldn't open file '" << outFileName << "'" << Log::Flush;
        throw "OutputHistoryToFile couldn't open file.";
    }

    Log::Info() << "Sim history data file is \'" << outFileName << "\'. " << Log::Flush;

    Str spaces(HEADER_SIZE_BYTES - 1, ' ');

    file << spaces << '\n';
}

OutputHistoryToFile::~OutputHistoryToFile() {
    auto *f = &outputFormatter;
    delete f;
};

void OutputHistoryToFile::_dump(bool integrationIsFinished) {
    if(integrationIsFinished){
        _printHeaderToFile({"phi"});

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
            Log::Info() << std::setprecision(3) << "Flushing " << (Real)Ti/Real(count)*100.0 << "%    " << Log::Flush;
        }

        file << outputFormatter(tHistory[int(Ti)]);

        const auto &fieldPair = spaceDataHistory[Ti];
        const DiscreteSpace &phiOut = *fieldPair.first;
        //const DiscreteSpace &ddtPhiOut = *fieldPair.second;

        file << outputFormatter(phiOut);
        //file << outputFormatter(ddtPhiOut);
    }

    file.flush();

    Log::Success() << "Flushed " << "100% " << Log::Flush;
}

void OutputHistoryToFile::_printHeaderToFile(std::vector<std::string> channelNames) {
    std::ostringstream oss;

    oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal+count);

    DimensionMetaData recDim = spaceFilter.getOutputDim();
    Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
    for(PosInt i=0; i<recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim[i];


    oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
    if(0) {
        oss << R"(, "data_channels": 2)";
        oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";
    } else {
        assert(channelNames.size() != 0);

        oss << R"(, "data_channels": )" << channelNames.size();
        oss << R"str(, "data_channel_names": ()str";
        for(auto name : channelNames)
            oss << "\"" << name << "\", ";
        oss << ") ";
    }

    oss << ", " << InterfaceManager::getInstance().renderAsPythonDictionaryEntries() << "}" << std::endl;

    const auto &s = oss.str();

    file.seekp(0);
    file.write(s.c_str(), (long)s.size());
}
