//
// Created by joao on 10/8/21.
//

#include "RtoROutputHistoryToFile.h"

#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"
#include "../../../../Core/Controller/InterfaceManager.h"

#include <iomanip>

const int HEADER_SIZE_BYTES = 2048;

namespace Slab::Math {

    using Core::FLog;

    RtoR::FOutputHistoryToFile::FOutputHistoryToFile(UInt stepsInterval,
                                                   FSpaceFilterBase *spaceFilter,
                                                   Str outputFileName)
            : FHistoryKeeper(stepsInterval, spaceFilter),
              outFileName(std::move(outputFileName)),
              outputFormatter(*(new FBinarySOF())) {
        this->Name = "Full (1+1) history output";

        file.open(outFileName, std::ios::out);

        if (!file) throw "FOutputHistoryToFile: nao abriu arquivo.";

        FLog::Info() << "Sim history will be saved in '" << outFileName << "'. " << FLog::Flush;

        file << Str(HEADER_SIZE_BYTES - 1, ' ') << '\n';
    }

    RtoR::FOutputHistoryToFile::~FOutputHistoryToFile() {
        auto *f = &outputFormatter;
        delete f;
    }

    void RtoR::FOutputHistoryToFile::_dump(bool integrationIsFinished) {
        if (integrationIsFinished) {
            _printHeaderToFile();

            auto shouldNotDump = !LastPacket.hasValidData();
            if (shouldNotDump) {
                file.close();
                return;
            }
        }

        FTimer timer;

        for (size_t Ti = 0; Ti < count; Ti++) {
            if (timer.GetElapsedTimeSeconds() > 1) {
                timer.Reset();
                FLog::Info() << std::setprecision(3) << "Flushing " << (DevFloat) Ti / DevFloat(count) * 100.0 << "%"
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

        FLog::Success() << "Flushed " << "100%" << FLog::Flush;
    }


    void RtoR::FOutputHistoryToFile::_printHeaderToFile() {
        // Allocator &builder = Allocator::getInstance();


        std::ostringstream oss;

        oss << R"(# {"Ver": 4, "lines_contain_timestamp": True, "outresT": )" << (countTotal + count);

        DimensionMetaData recDim = spaceFilter.getOutputDim();
        Str dimNames = "XYZUVWRSTABCDEFGHIJKLMNOPQ";
        for (UInt i = 0; i < recDim.getNDim(); i++) oss << ", \"outres" << dimNames[i] << "\": " << recDim.getN(i);


        oss << R"(, "data_type": ")" << outputFormatter.getFormatDescription() << "\"";
        oss << R"(, "data_channels": 2)";
        oss << R"str(, "data_channel_names": ("phi", "ddtphi") )str";

        oss << ", " << Core::FInterfaceManager::GetInstance().RenderAsPythonDictionaryEntries() << "}" << std::endl;

        const auto &s = oss.str();

        file.seekp(0);
        file.write(s.c_str(), (long) s.size());

    }

}