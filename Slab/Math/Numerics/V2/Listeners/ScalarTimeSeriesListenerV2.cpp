#include "ScalarTimeSeriesListenerV2.h"

namespace Slab::Math::Numerics::V2 {

    FScalarTimeSeriesListenerV2::FScalarTimeSeriesListenerV2(FExtractor extractor,
                                                             Str name,
                                                             std::optional<std::size_t> maxSamples)
    : Extractor(std::move(extractor))
    , Name(std::move(name))
    , MaxSamples(maxSamples) {
        if (!Extractor) throw Exception("FScalarTimeSeriesListenerV2 requires an extractor.");
    }

    auto FScalarTimeSeriesListenerV2::Collect(const FSimulationEventV2 &event) -> void {
        const auto valueOpt = Extractor(event);
        if (!valueOpt.has_value()) return;

        std::lock_guard lock(SamplesMutex);
        if (MaxSamples.has_value() && Samples.size() >= *MaxSamples) return;

        if (!Samples.empty() && Samples.back().PublishedVersion == event.PublishedVersion) return;

        Samples.push_back({
            .Cursor = event.Cursor,
            .Reason = event.Reason,
            .Value = *valueOpt,
            .PublishedVersion = event.PublishedVersion
        });
    }

    auto FScalarTimeSeriesListenerV2::OnRunStarted(const FSimulationEventV2 &initialEvent) -> void {
        Collect(initialEvent);
    }

    auto FScalarTimeSeriesListenerV2::OnSample(const FSimulationEventV2 &event) -> void {
        Collect(event);
    }

    auto FScalarTimeSeriesListenerV2::OnRunFinished(const FSimulationEventV2 &finalEvent) -> bool {
        Collect(finalEvent);
        return true;
    }

    auto FScalarTimeSeriesListenerV2::GetName() const -> Str {
        return Name;
    }

    auto FScalarTimeSeriesListenerV2::GetSampleCount() const -> std::size_t {
        std::lock_guard lock(SamplesMutex);
        return Samples.size();
    }

    auto FScalarTimeSeriesListenerV2::GetSamples() const -> Vector<FScalarTimeSeriesSampleV2> {
        std::lock_guard lock(SamplesMutex);
        return Samples;
    }

    auto FScalarTimeSeriesListenerV2::TryGetLatestSample() const -> std::optional<FScalarTimeSeriesSampleV2> {
        std::lock_guard lock(SamplesMutex);
        if (Samples.empty()) return std::nullopt;
        return Samples.back();
    }

    auto FScalarTimeSeriesListenerV2::Clear() -> void {
        std::lock_guard lock(SamplesMutex);
        Samples.clear();
    }

} // namespace Slab::Math::Numerics::V2
