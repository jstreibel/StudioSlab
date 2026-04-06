#ifndef STUDIOSLAB_LIVE_CONTROL_TOPICS_V2_H
#define STUDIOSLAB_LIVE_CONTROL_TOPICS_V2_H

#include "Core/SlabCore.h"
#include "Math/Formalism/Categories.h"

#include <mutex>
#include <optional>
#include <variant>

namespace Slab::Math::LiveControl::V2 {

    enum class EControlSemanticV2 {
        Level,
        Event
    };

    enum class EControlTimeDomainV2 {
        WallClockTime,
        SimulationTime,
        StepIndex
    };

    using FControlValueV2 = std::variant<bool, DevFloat, Real2D>;

    struct FControlTimestampV2 {
        EControlTimeDomainV2 Domain = EControlTimeDomainV2::WallClockTime;
        std::optional<DevFloat> WallClockSeconds = std::nullopt;
        std::optional<DevFloat> SimulationSeconds = std::nullopt;
        std::optional<UIntBig> Step = std::nullopt;
    };

    struct FControlSampleV2 {
        FControlValueV2 Value = false;
        EControlSemanticV2 Semantic = EControlSemanticV2::Level;
        FControlTimestampV2 Timestamp;
        UIntBig PublishedVersion = 0;
    };

    class FControlTopicV2 {
        Str Name;
        mutable std::mutex Mutex;
        std::optional<FControlSampleV2> LastSample = std::nullopt;

    public:
        explicit FControlTopicV2(Str name = "");

        [[nodiscard]] auto GetName() const -> Str;

        auto Publish(FControlSampleV2 sample) -> void;
        [[nodiscard]] auto HasSample() const -> bool;
        [[nodiscard]] auto TryGetLatestSample() const -> std::optional<FControlSampleV2>;

        template<typename TValue>
        [[nodiscard]] auto TryGetLatestValue() const -> std::optional<TValue> {
            const auto sample = TryGetLatestSample();
            if (!sample.has_value()) return std::nullopt;
            if (!std::holds_alternative<TValue>(sample->Value)) return std::nullopt;
            return std::get<TValue>(sample->Value);
        }

        [[nodiscard]] auto TryGetLatestBool() const -> std::optional<bool>;
        [[nodiscard]] auto TryGetLatestScalar() const -> std::optional<DevFloat>;
        [[nodiscard]] auto TryGetLatestPoint2D() const -> std::optional<Real2D>;
    };

    DefinePointers(FControlTopicV2)

} // namespace Slab::Math::LiveControl::V2

#endif // STUDIOSLAB_LIVE_CONTROL_TOPICS_V2_H
