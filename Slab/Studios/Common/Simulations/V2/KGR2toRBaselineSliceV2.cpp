#include "KGR2toRBaselineSliceV2.h"
#include "KGR2toRControlTopicsV2.h"

#include "../../Monitors/V2/KGR2toRBaselinePassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../V2SimulationRunners.h"

#include "Math/Data/V2/LiveControlHubV2.h"
#include "Math/Numerics/V2/Listeners/ListenerV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"

#include "Models/KleinGordon/R2toR/V2/KG-R2toR-Baseline-RecipeV2.h"

#include <algorithm>
#include <atomic>
#include <cmath>

namespace {

    class FThreadSafeGaussianForcingV2 final : public Slab::Math::R2toR::Function {
        std::atomic<Slab::DevFloat> XCenter;
        std::atomic<Slab::DevFloat> YCenter;
        std::atomic<Slab::DevFloat> Width;
        std::atomic<Slab::DevFloat> Amplitude;
        std::atomic<bool> bEnabled;

    public:
        FThreadSafeGaussianForcingV2(const Slab::DevFloat xCenter,
                                     const Slab::DevFloat yCenter,
                                     const Slab::DevFloat width,
                                     const Slab::DevFloat amplitude,
                                     const bool bInitiallyEnabled)
        : XCenter(xCenter)
        , YCenter(yCenter)
        , Width(std::max<Slab::DevFloat>(width, 1e-9))
        , Amplitude(amplitude)
        , bEnabled(bInitiallyEnabled) {
        }

        auto operator()(Slab::Math::Real2D x) const -> Slab::DevFloat override {
            if (!bEnabled.load(std::memory_order_relaxed)) return 0.0;

            const auto cx = XCenter.load(std::memory_order_relaxed);
            const auto cy = YCenter.load(std::memory_order_relaxed);
            const auto width = std::max<Slab::DevFloat>(Width.load(std::memory_order_relaxed), 1e-9);
            const auto amplitude = Amplitude.load(std::memory_order_relaxed);

            if (Slab::Common::AreEqual(amplitude, 0.0)) return 0.0;

            const auto dx = x.x - cx;
            const auto dy = x.y - cy;
            const auto r2 = dx * dx + dy * dy;
            const auto inv2Sigma2 = 1.0 / (2.0 * width * width);
            return amplitude * std::exp(-r2 * inv2Sigma2);
        }

        auto SetCenter(const Slab::DevFloat xCenter, const Slab::DevFloat yCenter) -> void {
            XCenter.store(xCenter, std::memory_order_relaxed);
            YCenter.store(yCenter, std::memory_order_relaxed);
        }

        auto SetWidth(const Slab::DevFloat width) -> void {
            Width.store(std::max<Slab::DevFloat>(width, 1e-9), std::memory_order_relaxed);
        }

        auto SetAmplitude(const Slab::DevFloat amplitude) -> void {
            Amplitude.store(amplitude, std::memory_order_relaxed);
        }

        auto SetEnabled(const bool bValue) -> void {
            bEnabled.store(bValue, std::memory_order_relaxed);
        }
    };

    class FLiveControlForcingBindingListenerV2 final : public Slab::Math::Numerics::V2::IListenerV2 {
        Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> ControlHub;
        Slab::TPointer<FThreadSafeGaussianForcingV2> ForcingSource;
        Slab::Str TopicPrefix;
        Slab::Str Name;

        auto ApplyLatestControls() -> void {
            using namespace Slab::Math::LiveControl::V2;

            if (ControlHub == nullptr || ForcingSource == nullptr) return;

            const auto topics = Slab::Studios::Common::Simulations::V2::BuildKG2DForcingTopicNamesV2(TopicPrefix);

            if (const auto centerTopic = ControlHub->FindTopic(topics.Center); centerTopic != nullptr) {
                if (const auto center = centerTopic->TryGetLatestPoint2D(); center.has_value()) {
                    ForcingSource->SetCenter(center->x, center->y);
                }
            }

            if (const auto ampTopic = ControlHub->FindTopic(topics.Amplitude); ampTopic != nullptr) {
                if (const auto amplitude = ampTopic->TryGetLatestScalar(); amplitude.has_value()) {
                    ForcingSource->SetAmplitude(*amplitude);
                }
            }

            if (const auto widthTopic = ControlHub->FindTopic(topics.Width); widthTopic != nullptr) {
                if (const auto width = widthTopic->TryGetLatestScalar(); width.has_value()) {
                    ForcingSource->SetWidth(*width);
                }
            }

            if (const auto enabledTopic = ControlHub->FindTopic(topics.Enabled); enabledTopic != nullptr) {
                if (const auto enabled = enabledTopic->TryGetLatestBool(); enabled.has_value()) {
                    ForcingSource->SetEnabled(*enabled);
                }
            }
        }

    public:
        FLiveControlForcingBindingListenerV2(const Slab::TPointer<Slab::Math::LiveControl::V2::FLiveControlHubV2> &controlHub,
                                             const Slab::TPointer<FThreadSafeGaussianForcingV2> &forcingSource,
                                             Slab::Str topicPrefix)
        : ControlHub(controlHub)
        , ForcingSource(forcingSource)
        , TopicPrefix(std::move(topicPrefix))
        , Name("KGR2toR LiveControl forcing binding V2") {
        }

        auto OnRunStarted(const Slab::Math::Numerics::V2::FSimulationEventV2 &initialEvent) -> void override {
            (void) initialEvent;
            ApplyLatestControls();
        }

        auto OnSample(const Slab::Math::Numerics::V2::FSimulationEventV2 &event) -> void override {
            (void) event;
            ApplyLatestControls();
        }

        auto OnRunFinished(const Slab::Math::Numerics::V2::FSimulationEventV2 &finalEvent) -> bool override {
            (void) finalEvent;
            ApplyLatestControls();
            return true;
        }

        [[nodiscard]] auto GetName() const -> Slab::Str override {
            return Name;
        }
    };

    auto BuildForcingControlSubscriptions(
        const Slab::Studios::Common::Simulations::V2::FR2toRBaselineExecutionConfig &cfg,
        Slab::TPointer<Slab::Math::R2toR::Function> &externalSourceOut)
        -> Slab::Vector<Slab::Math::Numerics::V2::FSubscriptionV2> {
        using namespace Slab::Math::Numerics::V2;

        externalSourceOut = nullptr;
        const auto bHasStaticForcing = cfg.bForcingEnabled || !Slab::Common::AreEqual(cfg.ForcingAmplitude, 0.0);
        const auto bNeedsForcingSource = cfg.bEnableLiveControlForcing || bHasStaticForcing;
        if (!bNeedsForcingSource) return {};

        auto forcingSource = Slab::New<FThreadSafeGaussianForcingV2>(
            cfg.ForcingXCenter,
            cfg.ForcingYCenter,
            cfg.ForcingWidth,
            cfg.ForcingAmplitude,
            cfg.bForcingEnabled);
        externalSourceOut = forcingSource;

        if (!cfg.bEnableLiveControlForcing) return {};

        if (cfg.ControlHub == nullptr) throw Exception("KGR2toR control binding requires a LiveControl hub.");
        if (cfg.ControlTopicPrefix.empty()) throw Exception("KGR2toR control binding requires a non-empty topic prefix.");

        auto bindingListener = Slab::New<FLiveControlForcingBindingListenerV2>(
            cfg.ControlHub,
            forcingSource,
            cfg.ControlTopicPrefix);

        Slab::Vector<FSubscriptionV2> subscriptions = {{
            Slab::New<FEveryNStepsTriggerV2>(std::max<Slab::UIntBig>(Slab::UIntBig(1), cfg.ControlSampleInterval)),
            bindingListener,
            EDeliveryModeV2::Synchronous,
            true,
            true
        }};

        return subscriptions;
    }

} // namespace

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeR2toRBaselineExecutionConfigV2(FR2toRBaselineExecutionConfig &cfg) -> void {
        if (cfg.N == 0) throw Exception("KGR2toR baseline requires N > 0.");
        if (cfg.Interval == 0) cfg.Interval = 1;
        if (cfg.MonitorInterval == 0) cfg.MonitorInterval = cfg.Interval;
        if (cfg.bEnableMonitorControlPublisher) {
            cfg.bEnableLiveControlForcing = true;
        }

        if (cfg.bEnableLiveControlForcing || cfg.bForcingEnabled || !Slab::Common::AreEqual(cfg.ForcingAmplitude, 0.0)) {
            if (cfg.ControlSampleInterval == 0) cfg.ControlSampleInterval = 1;
            if (cfg.ForcingWidth <= 0.0) throw Exception("KGR2toR control forcing width must be > 0.");
        }

        if ((cfg.bEnableLiveControlForcing || cfg.bEnableMonitorControlPublisher) && cfg.ControlHub == nullptr) {
            cfg.ControlHub = New<Math::LiveControl::V2::FLiveControlHubV2>();
        }

        if ((cfg.bEnableLiveControlForcing || cfg.bEnableMonitorControlPublisher) && cfg.ControlTopicPrefix.empty()) {
            throw Exception("KGR2toR control binding requires a non-empty topic prefix.");
        }
    }

    auto BuildR2toRBaselineRecipeConfigV2(const FR2toRBaselineExecutionConfig &cfg)
        -> Slab::Models::KGR2toR::Baseline::V2::FKGR2toRBaselineConfigV2 {
        Slab::Models::KGR2toR::Baseline::V2::FKGR2toRBaselineConfigV2 recipeCfg;
        recipeCfg.N = cfg.N;
        recipeCfg.L = cfg.L;
        recipeCfg.RDt = cfg.RDt;
        recipeCfg.Steps = cfg.Steps;
        recipeCfg.XCenter = cfg.XCenter;
        recipeCfg.YCenter = cfg.YCenter;
        recipeCfg.PulseWidth = cfg.PulseWidth;
        recipeCfg.PhiAmplitude = cfg.PhiAmplitude;
        recipeCfg.DPhiDtAmplitude = cfg.DPhiDtAmplitude;
        return recipeCfg;
    }

    auto BuildR2toRBaselineRecipeV2(const FR2toRBaselineExecutionConfig &cfg,
                                    const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::KGR2toR::Baseline::V2;
        using namespace Slab::Math::Numerics::V2;

        const auto recipeCfg = BuildR2toRBaselineRecipeConfigV2(cfg);
        TPointer<Math::R2toR::Function> externalSource = nullptr;
        const auto controlSubscriptions = BuildForcingControlSubscriptions(cfg, externalSource);

        auto recipe = New<FKGR2toRBaselineRecipeV2>(
            recipeCfg,
            std::max<UIntBig>(UIntBig(1), cfg.Interval),
            liveView,
            externalSource);
        recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);

        if (controlSubscriptions.empty()) return recipe;
        return New<FAppendedSubscriptionsRecipeV2>(recipe, controlSubscriptions);
    }

    auto BuildR2toRBaselinePassiveMonitorWindowV2(
            const FR2toRBaselineExecutionConfig &cfg,
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const TPointer<Math::Numerics::V2::FStateSnapshotListenerV2> &snapshotListener) -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("KGR2toR passive monitor requires a live view.");
        if (snapshotListener == nullptr) throw Exception("KGR2toR passive monitor requires a snapshot listener.");
        if (cfg.bEnableMonitorControlPublisher && cfg.ControlHub == nullptr) {
            throw Exception("KGR2toR monitor control publisher requires a LiveControl hub.");
        }

        return New<Slab::Studios::Common::Monitors::V2::FR2toRBaselinePassiveMonitorWindowV2>(
            liveView,
            snapshotListener,
            cfg.Steps,
            cfg.ControlHub,
            cfg.ControlTopicPrefix,
            cfg.bEnableMonitorControlPublisher,
            cfg.ForcingXCenter,
            cfg.ForcingYCenter,
            cfg.ForcingWidth,
            cfg.ForcingAmplitude,
            cfg.bForcingEnabled);
    }

    auto RunR2toRBaselineV2(const FR2toRBaselineExecutionConfig &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeR2toRBaselineExecutionConfigV2(runCfg);

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto baseRecipe = BuildR2toRBaselineRecipeV2(runCfg, liveView);
            auto snapshotListener = New<FStateSnapshotListenerV2>("KGR2toR baseline monitor snapshot listener V2");
            Vector<FSubscriptionV2> monitorSubscriptions = {{
                New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), runCfg.MonitorInterval)),
                snapshotListener,
                EDeliveryModeV2::LatestOnly,
                true,
                true
            }};
            auto recipe = New<FAppendedSubscriptionsRecipeV2>(baseRecipe, std::move(monitorSubscriptions));
            auto monitor = BuildR2toRBaselinePassiveMonitorWindowV2(runCfg, liveView, snapshotListener);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios KGR2toR Baseline V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch));
        }

        auto recipe = BuildR2toRBaselineRecipeV2(runCfg);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
