#ifndef STUDIOSLAB_LIVE_PARAMETER_CONTROL_V2_H
#define STUDIOSLAB_LIVE_PARAMETER_CONTROL_V2_H

#include "Core/SlabCore.h"
#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Numerics/V2/Listeners/ListenerV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <limits>
#include <optional>
#include <unordered_map>
#include <utility>

namespace Slab::Studios::Common::Simulations::V2 {

    enum class ELiveParameterMutabilityV2 {
        Const,
        RuntimeMutable
    };

    enum class ELiveParameterExposureV2 {
        Hidden,
        ReadOnlyExposed,
        WritableExposed
    };

    struct FLiveScalarParameterBindingV2 {
        Str Key;
        Str Label;
        Str Description;
        Str TopicName;
        std::optional<DevFloat> MinValue = std::nullopt;
        std::optional<DevFloat> MaxValue = std::nullopt;
        ELiveParameterMutabilityV2 Mutability = ELiveParameterMutabilityV2::Const;
        ELiveParameterExposureV2 Exposure = ELiveParameterExposureV2::ReadOnlyExposed;
        DevFloat InitialValue = 0.0;
        std::function<DevFloat()> ReadValue = nullptr;
        std::function<void(DevFloat)> WriteValue = nullptr;
    };

    struct FLiveControlBindingInfoV2 {
        Str Label;
        Str Description;
        std::optional<DevFloat> MinValue = std::nullopt;
        std::optional<DevFloat> MaxValue = std::nullopt;
        ELiveParameterMutabilityV2 Mutability = ELiveParameterMutabilityV2::Const;
        ELiveParameterExposureV2 Exposure = ELiveParameterExposureV2::ReadOnlyExposed;
    };

    inline auto LiveControlBindingRegistryMutex() -> std::mutex & {
        static std::mutex registryMutex;
        return registryMutex;
    }

    inline auto LiveControlBindingRegistry() -> std::unordered_map<Str, FLiveControlBindingInfoV2> & {
        static std::unordered_map<Str, FLiveControlBindingInfoV2> registry;
        return registry;
    }

    inline auto RegisterLiveScalarBindingV2(const FLiveScalarParameterBindingV2 &binding) -> void {
        if (binding.TopicName.empty()) return;
        std::lock_guard lock(LiveControlBindingRegistryMutex());
        LiveControlBindingRegistry()[binding.TopicName] = FLiveControlBindingInfoV2{
            binding.Label,
            binding.Description,
            binding.MinValue,
            binding.MaxValue,
            binding.Mutability,
            binding.Exposure
        };
    }

    [[nodiscard]] inline auto TryGetLiveControlBindingInfoV2(const Str &topicName)
        -> std::optional<FLiveControlBindingInfoV2> {
        if (topicName.empty()) return std::nullopt;
        std::lock_guard lock(LiveControlBindingRegistryMutex());
        const auto &registry = LiveControlBindingRegistry();
        const auto it = registry.find(topicName);
        if (it == registry.end()) return std::nullopt;
        return it->second;
    }

    [[nodiscard]] inline auto BuildLiveScalarParameterTopicV2(const Str &topicPrefix, const Str &key) -> Str {
        if (topicPrefix.empty()) return "lab/control/params/" + key;
        return topicPrefix + "/params/" + key;
    }

    [[nodiscard]] inline auto ClampLiveScalarParameterValueV2(const FLiveScalarParameterBindingV2 &binding,
                                                              const DevFloat value) -> DevFloat {
        auto clamped = value;
        if (binding.MinValue.has_value()) clamped = std::max(clamped, *binding.MinValue);
        if (binding.MaxValue.has_value()) clamped = std::min(clamped, *binding.MaxValue);
        return clamped;
    }

    [[nodiscard]] inline auto ReadLiveScalarParameterValueV2(const FLiveScalarParameterBindingV2 &binding) -> DevFloat {
        if (binding.ReadValue != nullptr) return ClampLiveScalarParameterValueV2(binding, binding.ReadValue());
        return ClampLiveScalarParameterValueV2(binding, binding.InitialValue);
    }

    inline auto PublishLiveScalarParameterValueV2(
        const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
        const Str &topicName,
        const DevFloat value) -> void {
        using namespace Math::LiveControl::V2;

        if (hub == nullptr || topicName.empty()) return;

        const auto now = std::chrono::duration<DevFloat>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        FControlTimestampV2 stamp;
        stamp.Domain = EControlTimeDomainV2::WallClockTime;
        stamp.WallClockSeconds = now;

        FControlSampleV2 sample;
        sample.Value = value;
        sample.Semantic = EControlSemanticV2::Level;
        sample.Timestamp = stamp;

        hub->GetOrCreateTopic(topicName)->Publish(sample);
    }

    inline auto PublishLiveScalarBindingsV2(const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
                                            const Vector<FLiveScalarParameterBindingV2> &bindings,
                                            const bool includeHidden = false) -> void {
        if (hub == nullptr) return;

        for (const auto &binding : bindings) {
            if (!includeHidden && binding.Exposure == ELiveParameterExposureV2::Hidden) continue;
            if (binding.TopicName.empty()) continue;

            RegisterLiveScalarBindingV2(binding);
            PublishLiveScalarParameterValueV2(
                hub,
                binding.TopicName,
                ReadLiveScalarParameterValueV2(binding));
        }
    }

    class FLiveScalarBindingsListenerV2 final : public Math::Numerics::V2::IListenerV2 {
        TPointer<Math::LiveControl::V2::FLiveControlHubV2> ControlHub;
        Vector<FLiveScalarParameterBindingV2> Bindings;
        Str Name;

        auto ApplyLatestValues() -> void {
            if (ControlHub == nullptr) return;

            for (const auto &binding : Bindings) {
                if (binding.Mutability != ELiveParameterMutabilityV2::RuntimeMutable) continue;
                if (binding.WriteValue == nullptr) continue;
                if (binding.TopicName.empty()) continue;

                const auto topic = ControlHub->FindTopic(binding.TopicName);
                if (topic == nullptr) continue;

                const auto value = topic->TryGetLatestScalar();
                if (!value.has_value()) continue;

                binding.WriteValue(ClampLiveScalarParameterValueV2(binding, *value));
            }
        }

    public:
        FLiveScalarBindingsListenerV2(TPointer<Math::LiveControl::V2::FLiveControlHubV2> controlHub,
                                      Vector<FLiveScalarParameterBindingV2> bindings,
                                      Str name = "Live scalar parameter binding V2")
        : ControlHub(std::move(controlHub))
        , Bindings(std::move(bindings))
        , Name(std::move(name)) {
        }

        auto OnRunStarted(const Math::Numerics::V2::FSimulationEventV2 &initialEvent) -> void override {
            (void) initialEvent;
            ApplyLatestValues();
        }

        auto OnSample(const Math::Numerics::V2::FSimulationEventV2 &event) -> void override {
            (void) event;
            ApplyLatestValues();
        }

        auto OnRunFinished(const Math::Numerics::V2::FSimulationEventV2 &finalEvent) -> bool override {
            (void) finalEvent;
            ApplyLatestValues();
            return true;
        }

        [[nodiscard]] auto GetName() const -> Str override {
            return Name;
        }
    };

    [[nodiscard]] inline auto BuildLiveScalarBindingSubscriptionsV2(
        const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &hub,
        const Vector<FLiveScalarParameterBindingV2> &bindings,
        const UIntBig sampleInterval,
        const Str &listenerName = "Live scalar parameter binding V2")
        -> Vector<Math::Numerics::V2::FSubscriptionV2> {
        using namespace Math::Numerics::V2;

        if (hub == nullptr || bindings.empty()) return {};

        const auto hasMutableBinding = std::ranges::any_of(bindings, [](const FLiveScalarParameterBindingV2 &binding) {
            return binding.Mutability == ELiveParameterMutabilityV2::RuntimeMutable &&
                binding.WriteValue != nullptr &&
                !binding.TopicName.empty();
        });
        if (!hasMutableBinding) return {};

        auto listener = New<FLiveScalarBindingsListenerV2>(hub, bindings, listenerName);

        Vector<FSubscriptionV2> subscriptions = {{
            New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), sampleInterval)),
            listener,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        return subscriptions;
    }

} // namespace Slab::Studios::Common::Simulations::V2

#endif
