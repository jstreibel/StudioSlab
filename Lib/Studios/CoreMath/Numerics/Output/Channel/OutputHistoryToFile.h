#ifndef OUTPUTFILE1D_H
#define OUTPUTFILE1D_H

#include "OutputHistory.h"
#include "Studios/Controller/Interface/CommonParameters.h"

#include "Studios/CoreMath/Numerics/Output/Format/BinarySOF.h"

const int HEADER_SIZE_BYTES = 2048;

class OutputHistoryToFile : public OutputHistory {

    const String outFileName;
    std::ofstream file;

    /*! I manage this. */
    OutputFormatterBase &outputFormatter;



public:

    OutputHistoryToFile(PosInt stepsInterval, SpaceFilterBase *spaceFilter,
                        Real endT, String  outputFileName,
                        OutputFormatterBase *outputFormatter = new BinarySOF());

    ~OutputHistoryToFile() override;

    auto description() const -> String override {return "full history output";}


private:

    void _printHeaderToFile();
    void _dump(bool integrationIsFinished) override;

public:
};

#endif // OUTPUTFILE1D_H
