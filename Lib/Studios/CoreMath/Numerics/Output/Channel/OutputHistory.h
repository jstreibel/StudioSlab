#ifndef OUTPUTHISTORY_H
#define OUTPUTHISTORY_H

#include "OutputChannel.h"
#include "Studios/CoreMath/Numerics/Output/Format/SpaceFilterBase.h"

#define M_GUESS 1024 //1048576 // 1024*1024

class OutputHistory : public OutputChannel
{
private:
    /** Dump is called whenever memory needs purging. Child class should dispose of all info contained in history,
     * because history will be purged. */
    virtual void _dump(bool integrationIsFinished) = 0;
    void _out(const OutputPacket &outInfo) final;

public:
    OutputHistory(size_t nStepsInterval, SpaceFilterBase *filter, double tEnd);
    ~OutputHistory() override;

    [[nodiscard]] auto getUtilMemLoadBytes() const -> long long unsigned int;
    auto shouldOutput(double t, long unsigned timestep) -> bool override;

    auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;

    auto renderMetaDataAsPythonDictionary() const -> String;
protected:
    SpaceFilterBase &spaceFilter;

    const double tEnd;

    std::vector<DiscreteSpacePair> spaceDataHistory; // pair: phi and dphidt
    VecFloat tHistory;
    size_t count;
    size_t countTotal;
};

#endif // OUTPUTHISTORY_H
