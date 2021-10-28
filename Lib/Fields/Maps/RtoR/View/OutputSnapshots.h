#ifndef OUTPUTSNAPSHOTS1D_H
#define OUTPUTSNAPSHOTS1D_H

#include "View/Base/OutputChannel.h"

class OutputSnapshot : public OutputChannel {

private:
    std::vector<size_t> snapSteps;
    String customFileDescription;

    size_t T_fileNamePrecision;
public:
    OutputSnapshot(const String &customFileDescription = "", const size_t fileNamePrecision = 4);

    String description() const override;

    ~OutputSnapshot();

    void addSnapshotStep(const size_t snapshotStep);

    static void doOutput(const OutputPacket &outInfo, const String& customFileDescription = "",
                         const size_t T_fileNamePrecision = 4);

    size_t computeNextRecStep() override;

protected:
    static void _outputToFile(DiscreteSpacePair spaceData, double t, const String &fileName);

protected:
    virtual bool shouldOutput(const double t, const long unsigned timeStep);

    virtual void _out(const OutputPacket &outInfo){
        OutputSnapshot::doOutput(outInfo, customFileDescription.c_str(), 4);
    }

public:
    virtual bool notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation);
};

#endif // OUTPUTSNAPSHOTS1D_H
