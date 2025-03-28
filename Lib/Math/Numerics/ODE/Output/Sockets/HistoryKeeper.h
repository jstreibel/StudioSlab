#ifndef OUTPUTHISTORY_H
#define OUTPUTHISTORY_H

#include "Math/Numerics/Socket.h"
#include "Math/Numerics/ODE/Output/Format/SpaceFilterBase.h"

#define M_GUESS 1024 //1048576 // 1024*1024

namespace Slab::Math {

    class HistoryKeeper : public Socket {
    private:
        /** Dump is called whenever memory needs purging. Child class should dispose of all info contained in history,
         * because history will be purged. */
        virtual void _dump(bool integrationIsFinished) = 0;

        void handleOutput(const OutputPacket &outInfo) final;

    protected:
        SpaceFilterBase &spaceFilter;

        Vector<DiscreteSpacePair> spaceDataHistory; // pair: phi and dphidt
        RealVector stepHistory;
        size_t count;
        size_t countTotal;

    public:
        HistoryKeeper(size_t nStepsInterval, SpaceFilterBase *filter);

        ~HistoryKeeper() override;

        [[nodiscard]] auto getUtilMemLoadBytes() const -> long long unsigned int;

        auto shouldOutput(long unsigned timestep) -> bool override;

        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;

        auto renderMetaDataAsPythonDictionary() const -> Str;
    };


}
#endif // OUTPUTHISTORY_H
