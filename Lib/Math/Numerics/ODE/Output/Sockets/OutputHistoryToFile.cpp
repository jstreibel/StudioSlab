
#include "OutputHistoryToFile.h"
#include "Core/Tools/Log.h"

#include "../../../../../Core/Controller/InterfaceManager.h"

#include "Utils/Timer.h"

#include <utility>
#include <iomanip>


namespace Slab::Math {

    const Str extension = ".osc";
#define outputFilename std::move(outputFileName + extension + (outputFormatter->isBinary()?"b":""))

    FOutputHistoryToFile::FOutputHistoryToFile(UInt stepsInterval,
                                               FSpaceFilterBase *spaceFilter,
                                               const Str &outputFileName,
                                               FOutputFormatterBase *outputFormatter)
    : FHistoryKeeper(stepsInterval, spaceFilter)
    , outFileName(outputFilename)
    , outputFormatter(*outputFormatter)
    {
        file.open(outFileName, std::ios::out | std::ios::binary | std::ios::trunc);

        using Core::FLog;

        if (!file) {
            FLog::Error() << "OutputHistoryToFile couldn't open file '" << outFileName << "'" << FLog::Flush;
            throw Exception("OutputHistoryToFile couldn't open file.");
        }

        const auto outputPath = outFileName.starts_with("/") ? outFileName : Common::GetPWD() + "/" + outFileName;
        FLog::Info() << "Sim history data file is \'" << outputPath << "\'. " << FLog::Flush;
        Str spaces(HEADER_SIZE_BYTES - 1, ' ');

        file << spaces << '\n';
    }

    FOutputHistoryToFile::~FOutputHistoryToFile() {
        auto *f = &outputFormatter;
        delete f;
    };

    void FOutputHistoryToFile::_dump(bool integrationIsFinished) {
        if (integrationIsFinished) {
            _printHeaderToFile({"phi", "ddtphi"});

            auto shouldNotDump = !LastPacket.hasValidData();
            if (shouldNotDump) {
                file.close();
                return;
            }
        }

        FTimer timer;

        using Core::FLog;

        for (size_t Ti = 0; Ti < count; Ti++) {
            if (timer.GetElapsedTimeSeconds() > 1) {
                timer.Reset();
                FLog::Info() << std::setprecision(3) << "Flushing " << (DevFloat) Ti / DevFloat(count) * 100.0 << "%    "
                            << FLog::Flush;
            }

            file << outputFormatter(stepHistory[int(Ti)]);

            const auto &fieldPair = spaceDataHistory[Ti];
            const DiscreteSpace &phiOut = *fieldPair.first;
            const DiscreteSpace &ddtPhiOut = *fieldPair.second;

            file << outputFormatter(phiOut);
            file << outputFormatter(ddtPhiOut);
        }

        file.flush();

        FLog::Success() << "Flushed " << "100% " << FLog::Flush;
    }

    void FOutputHistoryToFile::_printHeaderToFile(const Vector<std::string> &channelNames) {
        std::ostringstream oss;

        oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal + count);

        DimensionMetaData recDim = spaceFilter.getOutputDim();
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);

        oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";

        if (channelNames.empty())
            throw Exception("OSCB history must contain at least one data channel.");

        oss << R"(, "data_channels": )" << channelNames.size();
        oss << R"str(, "data_channel_names": ()str";
        for (const auto &name: channelNames)
            oss << "\"" << name << "\", ";
        oss << ") ";

        oss << ", " << Core::FInterfaceManager::GetInstance().RenderAsPythonDictionaryEntries() << "}";

        auto header = oss.str();
        if (header.size() >= HEADER_SIZE_BYTES)
            throw Exception("OSCB header exceeds the reserved " + ToStr(HEADER_SIZE_BYTES) + " bytes.");

        header.resize(HEADER_SIZE_BYTES - 1, ' ');
        header.push_back('\n');

        file.seekp(0, std::ios::beg);
        file.write(header.data(), static_cast<std::streamsize>(header.size()));
        if (!file)
            throw Exception("Failed to write OSCB header.");

        // Partial history dumps already live after the reserved header. Resume at
        // the physical end so the final buffered block is appended, not written
        // over the header padding or an earlier block.
        file.seekp(0, std::ios::end);
    }


}
