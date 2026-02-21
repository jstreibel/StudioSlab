//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTOROUTPUTHISTORYTOFILE_H
#define FIELDS_RTOROUTPUTHISTORYTOFILE_H

#include "Math/Numerics/ODE/Output/Sockets/HistoryKeeper.h"
#include "Math/Numerics/ODE/Output/Format/OutputFormatterBase.h"

#include <fstream>

namespace Slab::Math::RtoR {
    class FOutputHistoryToFile : public FHistoryKeeper {
        void _dump(bool integrationIsFinished) override;
        void _printHeaderToFile();

        const Str outFileName;
        std::ofstream file;

        FOutputFormatterBase &outputFormatter;

    public:
        FOutputHistoryToFile(UInt stepsInterval, FSpaceFilterBase *spaceFilter, Str  outputFileName);

        ~FOutputHistoryToFile() override;

    };

    using OutputHistoryToFile [[deprecated("Use FOutputHistoryToFile")]] = FOutputHistoryToFile;
}


#endif //FIELDS_RTOROUTPUTHISTORYTOFILE_H
