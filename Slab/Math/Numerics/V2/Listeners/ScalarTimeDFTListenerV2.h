#ifndef STUDIOSLAB_SCALAR_TIME_DFT_LISTENER_V2_H
#define STUDIOSLAB_SCALAR_TIME_DFT_LISTENER_V2_H

#include "ListenerV2.h"

#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

#include <functional>

namespace Slab::Math::Numerics::V2 {

    class FScalarTimeDFTListenerV2 final : public IListenerV2 {
    public:
        using FExtractor = std::function<std::optional<DevFloat>(const FSimulationEventV2 &)>;

    private:
        Vector<DevFloat> SampleTimes;
        Vector<DevFloat> SampleValues;
        TPointer<Math::RtoR::NumericFunction> Magnitudes = nullptr;
        FExtractor Extractor;
        Str Name;

        auto Collect(const FSimulationEventV2 &event) -> void;
        auto RecomputeDFT() -> void;

    public:
        explicit FScalarTimeDFTListenerV2(FExtractor extractor,
                                          Str name = "Scalar Time DFT Listener V2");

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto GetSampleCount() const -> size_t;
        [[nodiscard]] auto GetDFTMagnitudes() const -> TPointer<Math::RtoR::NumericFunction>;
        [[nodiscard]] auto GetSampleTimes() const -> const Vector<DevFloat> &;
        [[nodiscard]] auto GetSampleValues() const -> const Vector<DevFloat> &;
    };

    DefinePointers(FScalarTimeDFTListenerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SCALAR_TIME_DFT_LISTENER_V2_H
