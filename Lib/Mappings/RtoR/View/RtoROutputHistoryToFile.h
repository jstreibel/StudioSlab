//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTOROUTPUTHISTORYTOFILE_H
#define FIELDS_RTOROUTPUTHISTORYTOFILE_H

#include "Phys/Numerics/Output/Plugs/HistoryKeeper.h"
#include "Phys/Numerics/Output/Format/OutputFormatterBase.h"

namespace RtoR {
    class OutputHistoryToFile : public HistoryKeeper {
        void _dump(bool integrationIsFinished) override;
        void _printHeaderToFile();

        const Str outFileName;
        std::ofstream file;

        OutputFormatterBase &outputFormatter;

    public:
        OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter, Real endT, Str  outputFileName);

        ~OutputHistoryToFile() override;

    };
}


#endif //FIELDS_RTOROUTPUTHISTORYTOFILE_H
