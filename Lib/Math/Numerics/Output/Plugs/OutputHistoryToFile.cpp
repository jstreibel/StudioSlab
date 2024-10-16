
#include "OutputHistoryToFile.h"
#include "Core/Tools/Log.h"

#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include <Utils/Timer.h>

#include <utility>
#include <iomanip>


namespace Slab::Math {

    const Str extension = ".osc";
#define outputFilename std::move(outputFileName + extension + (outputFormatter->isBinary()?"b":""))

    OutputHistoryToFile::OutputHistoryToFile(UInt stepsInterval,
                                             SpaceFilterBase *spaceFilter,
                                             Real endT,
                                             const Str &outputFileName, OutputFormatterBase *outputFormatter)
            : HistoryKeeper(stepsInterval, spaceFilter, endT), outFileName(outputFilename),
              outputFormatter(*outputFormatter) {
        file.open(outFileName, std::ios::out);

        using Core::Log;

        if (!file) {
            Log::Error() << "OutputHistoryToFile couldn't open file '" << outFileName << "'" << Log::Flush;
            throw Exception("OutputHistoryToFile couldn't open file.");
        }

        Log::Info() << "Sim history data file is \'" << Common::GetPWD() << "/" << outFileName << "\'. " << Log::Flush;
        Str spaces(HEADER_SIZE_BYTES - 1, ' ');

        file << spaces << '\n';
    }

    OutputHistoryToFile::~OutputHistoryToFile() {
        auto *f = &outputFormatter;
        delete f;
    };

    void OutputHistoryToFile::_dump(bool integrationIsFinished) {
        if (integrationIsFinished) {
            _printHeaderToFile({"phi"});

            auto shouldNotDump = !lastPacket.hasValidData();
            if (shouldNotDump) {
                file.close();
                return;
            }
        }

        Timer timer;

        using Core::Log;

        for (size_t Ti = 0; Ti < count; Ti++) {
            if (timer.getElTime_sec() > 1) {
                timer.reset();
                Log::Info() << std::setprecision(3) << "Flushing " << (Real) Ti / Real(count) * 100.0 << "%    "
                            << Log::Flush;
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

    void OutputHistoryToFile::_printHeaderToFile(Vector<std::string> channelNames) {
        std::ostringstream oss;


        oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal + count);

        fix dummyL = 1.23424;
        NOT_IMPLEMENTED
        DimensionMetaData recDim = spaceFilter.getOutputDim(dummyL);
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);


        oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
        if (0) {
            oss << R"(, "data_channels": 2)";
            oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";
        } else {
            assert(channelNames.size() != 0);

            oss << R"(, "data_channels": )" << channelNames.size();
            oss << R"str(, "data_channel_names": ()str";
            for (auto name: channelNames)
                oss << "\"" << name << "\", ";
            oss << ") ";
        }

        oss << ", " << Core::CLInterfaceManager::getInstance().renderAsPythonDictionaryEntries() << "}" << std::endl;

        const auto &s = oss.str();

        file.seekp(0);
        file.write(s.c_str(), (long) s.size());
    }


}