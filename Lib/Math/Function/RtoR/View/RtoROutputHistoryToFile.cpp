//
// Created by joao on 10/8/21.
//

#include "RtoROutputHistoryToFile.h"

#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include <iomanip>

const int HEADER_SIZE_BYTES = 2048;

namespace Slab::Math {

    using Core::Log;

    RtoR::OutputHistoryToFile::OutputHistoryToFile(UInt stepsInterval,
                                                   SpaceFilterBase *spaceFilter,
                                                   Str outputFileName)
            : HistoryKeeper(stepsInterval, spaceFilter),
              outFileName(std::move(outputFileName)),
              outputFormatter(*(new BinarySOF())) {
        this->name = "Full (1+1) history output";

        file.open(outFileName, std::ios::out);

        if (!file) throw "OutputHistoryToFile: nao abriu arquivo.";

        Log::Info() << "Sim history will be saved in '" << outFileName << "'. " << Log::Flush;

        file << Str(HEADER_SIZE_BYTES - 1, ' ') << '\n';
    }

    RtoR::OutputHistoryToFile::~OutputHistoryToFile() {
        auto *f = &outputFormatter;
        delete f;
    }

    void RtoR::OutputHistoryToFile::_dump(bool integrationIsFinished) {
        if (integrationIsFinished) {
            _printHeaderToFile();

            auto shouldNotDump = !lastPacket.hasValidData();
            if (shouldNotDump) {
                file.close();
                return;
            }
        }

        Timer timer;

        for (size_t Ti = 0; Ti < count; Ti++) {
            if (timer.getElTime_sec() > 1) {
                timer.reset();
                Log::Info() << std::setprecision(3) << "Flushing " << (Real) Ti / Real(count) * 100.0 << "%"
                            << Log::Flush;
            }

            file << outputFormatter(stepHistory[int(Ti)]);

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

        oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal + count);

        DimensionMetaData recDim = spaceFilter.getOutputDim();
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);


        oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
        oss << R"(, "data_channels": 2)";
        oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";

        oss << ", " << Core::CLInterfaceManager::getInstance().renderAsPythonDictionaryEntries() << "}" << std::endl;

        const auto &s = oss.str();

        file.seekp(0);
        file.write(s.c_str(), (long) s.size());

    }

}