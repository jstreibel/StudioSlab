#ifndef OUTPUTHISTORY_H
#define OUTPUTHISTORY_H

#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Phys/Numerics/Output/Format/SpaceFilterBase.h"

#define M_GUESS 1024 //1048576 // 1024*1024

class HistoryKeeper : public Numerics::OutputSystem::Socket
{
private:
    /** Dump is called whenever memory needs purging. Child class should dispose of all info contained in history,
     * because history will be purged. */
    virtual void _dump(bool integrationIsFinished) = 0;
    void _out(const OutputPacket &outInfo) final;

public:
    HistoryKeeper(size_t nStepsInterval, SpaceFilterBase *filter, Real tEnd);
    ~HistoryKeeper() override;

    [[nodiscard]] auto getUtilMemLoadBytes() const -> long long unsigned int;
    auto shouldOutput(Real t, long unsigned timestep) -> bool override;

    auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;

    auto renderMetaDataAsPythonDictionary() const -> String;
protected:
    SpaceFilterBase &spaceFilter;

    const Real tEnd;

    std::vector<DiscreteSpacePair> spaceDataHistory; // pair: phi and dphidt
    VecFloat tHistory;
    size_t count;
    size_t countTotal;
};

#endif // OUTPUTHISTORY_H
