#ifndef OUTPUTFILE1D_H
#define OUTPUTFILE1D_H

#include "OutputHistory.h"
#include "Studios/Controller/Interface/CommonParameters.h"

#include "Phys/Numerics/Output/Format/BinarySOF.h"

/* TODO this should actually be computed in execution, and be on the first 4 bytes (32 bit unsigned int) of the file,
        indicating header size. */
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


private:

    void _printHeaderToFile(std::vector<std::string> channelNames);
    void _dump(bool integrationIsFinished) override;

public:
};

#endif // OUTPUTFILE1D_H
