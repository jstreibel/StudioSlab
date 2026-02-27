#include "LiveControlTopicsV2.h"

namespace Slab::Math::LiveControl::V2 {

    FControlTopicV2::FControlTopicV2(Str name)
    : Name(std::move(name)) {
    }

    auto FControlTopicV2::GetName() const -> Str {
        return Name;
    }

    auto FControlTopicV2::Publish(FControlSampleV2 sample) -> void {
        std::lock_guard lock(Mutex);

        const auto nextVersion = LastSample.has_value()
            ? (LastSample->PublishedVersion + 1)
            : UIntBig(1);
        sample.PublishedVersion = nextVersion;
        LastSample = std::move(sample);
    }

    auto FControlTopicV2::HasSample() const -> bool {
        std::lock_guard lock(Mutex);
        return LastSample.has_value();
    }

    auto FControlTopicV2::TryGetLatestSample() const -> std::optional<FControlSampleV2> {
        std::lock_guard lock(Mutex);
        return LastSample;
    }

    auto FControlTopicV2::TryGetLatestBool() const -> std::optional<bool> {
        return TryGetLatestValue<bool>();
    }

    auto FControlTopicV2::TryGetLatestScalar() const -> std::optional<DevFloat> {
        return TryGetLatestValue<DevFloat>();
    }

    auto FControlTopicV2::TryGetLatestPoint2D() const -> std::optional<Real2D> {
        return TryGetLatestValue<Real2D>();
    }

} // namespace Slab::Math::LiveControl::V2
