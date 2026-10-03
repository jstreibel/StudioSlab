#include <catch2/catch_test_macros.hpp>

#include "Math/Numerics/ODE/Output/Sockets/HistoryKeeper.h"
#include "Math/VectorSpace/Impl/DiscreteSpaceCPU.h"

namespace {
using namespace Slab;
using namespace Slab::Math;

class FFixedHistoryFilter final : public FSpaceFilterBase {
    DimensionMetaData Dimension{{2U}, {1.0}};

  public:
    auto operator()(const FOutputPacket&) -> DiscreteSpacePair override {
        auto* phi = new DiscreteSpaceCPU(Dimension);
        auto* dPhiDt = new DiscreteSpaceCPU(Dimension);
        phi->getHostData()[0] = 1.0;
        phi->getHostData()[1] = 2.0;
        dPhiDt->getHostData()[0] = 3.0;
        dPhiDt->getHostData()[1] = 4.0;
        return {phi, dPhiDt};
    }

    auto getOutputDim() const -> DimensionMetaData override {
        return Dimension;
    }
};

class FRecordingHistory final : public FHistoryKeeper {
  public:
    Vector<FRealVector> Dumps;

    FRecordingHistory() : FHistoryKeeper(1, new FFixedHistoryFilter(), 1) {}

  private:
    void _dump(bool) override {
        Dumps.push_back(stepHistory);
    }
};
} // namespace

TEST_CASE("HistoryKeeper keeps steps aligned across partial dumps", "[OSCB][History]") {
    FRecordingHistory history;

    history.Output(Slab::Math::FOutputPacket(nullptr, 3));
    history.Output(Slab::Math::FOutputPacket(nullptr, 7));

    REQUIRE(history.Dumps.size() == 1);
    REQUIRE(history.Dumps[0] == Slab::FRealVector{3.0});

    history.NotifyIntegrationHasFinished(Slab::Math::FOutputPacket(nullptr, 7));

    REQUIRE(history.Dumps.size() == 2);
    REQUIRE(history.Dumps[1] == Slab::FRealVector{7.0});
    REQUIRE(history.getUtilMemLoadBytes() == 0);
}
