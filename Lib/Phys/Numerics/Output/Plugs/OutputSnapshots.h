#ifndef OUTPUTSNAPSHOTS1D_H
#define OUTPUTSNAPSHOTS1D_H

#include "Phys/Numerics/Output/Plugs/Socket.h"

class OutputSnapshot : public Numerics::OutputSystem::Socket {

private:
    std::vector<size_t> snapSteps;
    Str customFileDescription;

    size_t T_fileNamePrecision;
public:
    OutputSnapshot(const NumericParams &params, const Str &customFileDescription = "", const size_t fileNamePrecision = 4);

    ~OutputSnapshot();

    void addSnapshotStep(const size_t snapshotStep);

    static void doOutput(const OutputPacket &outInfo, const Str& customFileDescription = "",
                         const size_t T_fileNamePrecision = 4);

    size_t computeNextRecStep() override;

protected:
    static void _outputToFile(DiscreteSpacePair spaceData, Real t, const Str &fileName);

protected:
    virtual bool shouldOutput(const Real t, const long unsigned timeStep) override;

    virtual void _out(const OutputPacket &outInfo) override {
        OutputSnapshot::doOutput(outInfo, customFileDescription.c_str(), 4);
    }
};

#endif // OUTPUTSNAPSHOTS1D_H
