#ifndef OUTPUTFILE1D_H
#define OUTPUTFILE1D_H

#include "HistoryKeeper.h"
#include "../../../../../Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Numerics/ODE/Output/Format/BinarySOF.h"

#include <fstream>

/* TODO this would better be computed in execution, and be on the first 4 bytes (32 bit unsigned int) of the file,
        indicating header size. */
const int HEADER_SIZE_BYTES = 2048;


namespace Slab::Math {

    class OutputHistoryToFile : public HistoryKeeper {

        const Str outFileName;
        std::ofstream file;

        /*! I manage this. */
        FOutputFormatterBase &outputFormatter;


    public:

        OutputHistoryToFile(UInt stepsInterval, FSpaceFilterBase *spaceFilter,
                            const Str &outputFileName,
                            FOutputFormatterBase *outputFormatter = new FBinarySOF());

        ~OutputHistoryToFile() override;


    private:

        void _printHeaderToFile(Vector<std::string> channelNames);

        void _dump(bool integrationIsFinished) override;

    public:
    };


}


#endif // OUTPUTFILE1D_H
