#ifndef STUDIOSLAB_SCALAR_TIME_SERIES_LISTENER_V2_H
#define STUDIOSLAB_SCALAR_TIME_SERIES_LISTENER_V2_H

#include "ListenerV2.h"

#include <functional>
#include <mutex>

namespace Slab::Math::Numerics::V2 {

    struct FScalarTimeSeriesSampleV2 {
        FSimulationCursorV2 Cursor;
        EEventReasonV2 Reason = EEventReasonV2::Scheduled;
        DevFloat Value = 0.0;
        UIntBig PublishedVersion = 0;
    };

    class FScalarTimeSeriesListenerV2 final : public IListenerV2 {
    public:
        using FExtractor = std::function<std::optional<DevFloat>(const FSimulationEventV2 &)>;

    private:
        mutable std::mutex SamplesMutex;
        Vector<FScalarTimeSeriesSampleV2> Samples;
        FExtractor Extractor;
        Str Name;
        std::optional<std::size_t> MaxSamples = std::nullopt;

        auto Collect(const FSimulationEventV2 &event) -> void;

    public:
        explicit FScalarTimeSeriesListenerV2(FExtractor extractor,
                                             Str name = "Scalar Time Series Listener V2",
                                             std::optional<std::size_t> maxSamples = std::nullopt);

        auto OnRunStarted(const FSimulationEventV2 &initialEvent) -> void override;
        auto OnSample(const FSimulationEventV2 &event) -> void override;
        auto OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool override;

        [[nodiscard]] auto GetName() const -> Str override;
        [[nodiscard]] auto GetSampleCount() const -> std::size_t;
        [[nodiscard]] auto GetSamples() const -> Vector<FScalarTimeSeriesSampleV2>;
        [[nodiscard]] auto TryGetLatestSample() const -> std::optional<FScalarTimeSeriesSampleV2>;
        auto Clear() -> void;
    };

    DefinePointers(FScalarTimeSeriesListenerV2)

} // namespace Slab::Math::Numerics::V2

#endif // STUDIOSLAB_SCALAR_TIME_SERIES_LISTENER_V2_H
