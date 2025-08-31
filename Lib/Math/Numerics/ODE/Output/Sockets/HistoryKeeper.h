#ifndef OUTPUTHISTORY_H
#define OUTPUTHISTORY_H

#include "Math/Numerics/OutputChannel.h"
#include "Math/Numerics/ODE/Output/Format/SpaceFilterBase.h"

#define M_GUESS 1024 //1048576 // 1024*1024

namespace Slab::Math {

    class HistoryKeeper : public FOutputChannel {
    private:
        /** Dump is called whenever memory needs purging. Child class should dispose of all info contained in history,
         * because history will be purged. */
        virtual void _dump(bool integrationIsFinished) = 0;

        void HandleOutput(const FOutputPacket &outInfo) final;

    protected:
        SpaceFilterBase &spaceFilter;

        Vector<DiscreteSpacePair> spaceDataHistory; // pair: phi and dphidt
        FRealVector stepHistory;
        size_t count;
        size_t countTotal;

    public:
        HistoryKeeper(size_t nStepsInterval, SpaceFilterBase *filter);

        ~HistoryKeeper() override;

        [[nodiscard]] auto getUtilMemLoadBytes() const -> long long unsigned int;

        auto ShouldOutput(long unsigned timestep) -> bool override;

        auto NotifyIntegrationHasFinished(const FOutputPacket &theVeryLastOutputInformation) -> bool override;

        auto renderMetaDataAsPythonDictionary() const -> Str;
    };


}
#endif // OUTPUTHISTORY_H
