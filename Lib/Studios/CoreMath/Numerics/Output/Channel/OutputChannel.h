#ifndef OUTPUT_H
#define OUTPUT_H

#include "Studios/CoreMath/Numerics/Output/Util/OutputPacket.h"

#define INT_BASED 1
#define FLOAT_BASED 2

#define SHOULD_OUTPUT___MODE INT_BASED

class OutputChannel {

    virtual void _out(const OutputPacket &outputPacket) = 0;

public:
    virtual ~OutputChannel() = default;

    virtual auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool = 0;
    virtual auto description() const -> String = 0;

    virtual auto computeNextRecStep() -> size_t;

    /*!
     * This function allows child classes to give additional info on whether info should be output, or completely
     * override this information. It is basically used by the class NumericalIntegration, which needs to know when to output
     * stuff.
     * @param t The Real-valued time at which the requirer wants to know if output should be generated.
     * @param timestep The timestemp at which the requirer wants to know if output should be generated. Should match 't'
     * above.
     * @return
     */
    virtual auto shouldOutput(double t, long unsigned timestep) -> bool;


protected:

    int nStepsBetweenRecordings; // numero de passos entre uma gravacao e outra.
    OutputPacket lastInfo;


public:

    explicit OutputChannel(int nStepsInterval = 1);

    auto getLastT() -> double;
    auto getNSteps() const -> int;

    void output(const OutputPacket &outInfo);

};

typedef std::vector<OutputChannel*> OutputCollection;

#endif
