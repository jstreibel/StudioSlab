//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_RTOROUTPUTHISTORYTOFILE_H
#define FIELDS_RTOROUTPUTHISTORYTOFILE_H

#include <Studios/CoreMath/Numerics/Output/Channel/OutputHistory.h>

#include <Studios/CoreMath/Numerics/Output/Format/OutputFormatterBase.h>

namespace RtoR {
    class OutputHistoryToFile : public OutputHistory {
        void _dump(bool integrationIsFinished) override;
        void _printHeaderToFile();

        const String outFileName;
        std::ofstream file;

        OutputFormatterBase &outputFormatter;

    public:
        OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter, Real endT, String  outputFileName);

        ~OutputHistoryToFile() override;

        auto description() const -> String override {return "full (1+1) history output";}
    };
}


#endif //FIELDS_RTOROUTPUTHISTORYTOFILE_H
