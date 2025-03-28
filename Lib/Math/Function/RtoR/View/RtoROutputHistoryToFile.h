//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTOROUTPUTHISTORYTOFILE_H
#define FIELDS_RTOROUTPUTHISTORYTOFILE_H

#include "Math/Numerics/ODE/Output/Sockets/HistoryKeeper.h"
#include "Math/Numerics/ODE/Output/Format/OutputFormatterBase.h"

#include <fstream>

namespace Slab::Math::RtoR {
    class OutputHistoryToFile : public HistoryKeeper {
        void _dump(bool integrationIsFinished) override;
        void _printHeaderToFile();

        const Str outFileName;
        std::ofstream file;

        OutputFormatterBase &outputFormatter;

    public:
        OutputHistoryToFile(UInt stepsInterval, SpaceFilterBase *spaceFilter, Str  outputFileName);

        ~OutputHistoryToFile() override;

    };
}


#endif //FIELDS_RTOROUTPUTHISTORYTOFILE_H
