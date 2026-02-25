#include "KGRtoRPlaneWavesSliceV2.h"

#include "../../Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.h"
#include "../../NumericsV2TaskUtils.h"
#include "../../SessionLiveViewStatsV2.h"
#include "../../V2SimulationRunners.h"

#include "Math/Data/V2/SessionLiveViewV2.h"
#include "Math/Numerics/V2/Listeners/CursorHistoryListenerV2.h"
#include "Math/Numerics/V2/Listeners/ScalarTimeDFTListenerV2.h"
#include "Math/Numerics/V2/Listeners/StateSnapshotListenerV2.h"
#include "Math/Numerics/V2/Runtime/AppendedSubscriptionsRecipeV2.h"
#include "Math/Numerics/V2/Task/NumericTaskV2.h"
#include "Math/Numerics/V2/Scheduling/EveryNStepsTriggerV2.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Models/KleinGordon/RtoR/LinearStepping/V2/KG-RtoR-PlaneWaves-RecipeV2.h"

#include <algorithm>
#include <iostream>

namespace Slab::Studios::Common::Simulations::V2 {

    auto FinalizeRtoRPlaneWavesExecutionConfigV2(FRtoRPlaneWavesExecutionConfig &cfg) -> void {
        if (cfg.N == 0) throw Exception("RtoR requires N > 0.");

        if (cfg.Dt <= 0.0) {
            const auto h = cfg.L / static_cast<DevFloat>(cfg.N);
            cfg.Dt = static_cast<DevFloat>(0.1) * h; // Legacy KG default: dt/h = 0.1
        }

        if (cfg.DFTProbeIndex.has_value() && *cfg.DFTProbeIndex >= cfg.N) {
            throw Exception("RtoR DFT probe index must be < N.");
        }
        if (cfg.DFTInterval == 0) {
            cfg.DFTInterval = std::max<UIntBig>(UIntBig(1), cfg.Interval);
        }
    }

    namespace {

        struct FRtoRPlaneWavesAnalysisHandlesV2 {
            Math::Numerics::V2::FCursorHistoryListenerV2_ptr CursorHistory = nullptr;
            Math::Numerics::V2::FStateSnapshotListenerV2_ptr FinalSnapshot = nullptr;
            Math::Numerics::V2::FScalarTimeDFTListenerV2_ptr ScalarDFT = nullptr;
            std::optional<UInt> DFTProbeIndex = std::nullopt;

            [[nodiscard]] auto HasAny() const -> bool {
                return CursorHistory != nullptr || FinalSnapshot != nullptr || ScalarDFT != nullptr;
            }
        };

        auto BuildPhiProbeExtractor(const UInt probeIndex) -> Math::Numerics::V2::FScalarTimeDFTListenerV2::FExtractor {
            return [probeIndex](const Math::Numerics::V2::FSimulationEventV2 &event) -> std::optional<DevFloat> {
                if (event.State == nullptr) return std::nullopt;

                auto kgState = std::dynamic_pointer_cast<const Slab::Models::KGRtoR::FEquationState>(event.State);
                if (kgState == nullptr) return std::nullopt;

                auto *phiBase = &kgState->getPhi();
                auto *phiNumeric = dynamic_cast<Math::RtoR::NumericFunction *>(phiBase);
                if (phiNumeric == nullptr) return std::nullopt;
                if (probeIndex >= phiNumeric->N) return std::nullopt;

                const auto &host = phiNumeric->getSpace().getHostData(true);
                if (probeIndex >= host.size()) return std::nullopt;
                return host[probeIndex];
            };
        }

        auto BuildAnalysisSubscriptions(const FRtoRPlaneWavesExecutionConfig &cfg,
                                        FRtoRPlaneWavesAnalysisHandlesV2 &handles)
            -> Vector<Math::Numerics::V2::FSubscriptionV2> {
            using namespace Math::Numerics::V2;

            Vector<FSubscriptionV2> subscriptions;

            if (cfg.bHistorySummary) {
                handles.CursorHistory = New<FCursorHistoryListenerV2>("KGRtoR Plane Waves Cursor History V2");
                subscriptions.push_back({
                    New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), cfg.Interval)),
                    handles.CursorHistory,
                    EDeliveryModeV2::Synchronous,
                    true,
                    true
                });
            }

            if (cfg.bSnapshotSummary) {
                handles.FinalSnapshot = New<FStateSnapshotListenerV2>("KGRtoR Plane Waves Final Snapshot V2");
                subscriptions.push_back({
                    nullptr,
                    handles.FinalSnapshot,
                    EDeliveryModeV2::Synchronous,
                    false,
                    true
                });
            }

            if (cfg.DFTProbeIndex.has_value()) {
                handles.DFTProbeIndex = cfg.DFTProbeIndex;
                handles.ScalarDFT = New<FScalarTimeDFTListenerV2>(
                    BuildPhiProbeExtractor(*cfg.DFTProbeIndex),
                    "KGRtoR Plane Waves Scalar DFT V2");
                subscriptions.push_back({
                    New<FEveryNStepsTriggerV2>(std::max<UIntBig>(UIntBig(1), cfg.DFTInterval)),
                    handles.ScalarDFT,
                    EDeliveryModeV2::Synchronous,
                    false,
                    true
                });
            }

            return subscriptions;
        }

        auto PrintHistorySummary(const FRtoRPlaneWavesAnalysisHandlesV2 &handles) -> void {
            if (handles.CursorHistory == nullptr) return;

            const auto &samples = handles.CursorHistory->GetSamples();
            std::cout << "  CursorHistory: samples=" << samples.size();
            if (!samples.empty()) {
                const auto &first = samples.front();
                const auto &last = samples.back();
                std::cout << " first(step=" << first.Cursor.Step;
                if (first.Cursor.SimulationTime.has_value()) std::cout << ", t=" << *first.Cursor.SimulationTime;
                std::cout << ", reason=" << Slab::Studios::Common::ToDisplayString(first.Reason) << ")";
                std::cout << " last(step=" << last.Cursor.Step;
                if (last.Cursor.SimulationTime.has_value()) std::cout << ", t=" << *last.Cursor.SimulationTime;
                std::cout << ", reason=" << Slab::Studios::Common::ToDisplayString(last.Reason) << ")";
            }
            std::cout << '\n';
        }

        auto PrintSnapshotSummary(const FRtoRPlaneWavesAnalysisHandlesV2 &handles) -> void {
            if (handles.FinalSnapshot == nullptr) return;

            std::cout << "  FinalSnapshot: ";
            if (!handles.FinalSnapshot->HasSnapshot()) {
                std::cout << "none\n";
                return;
            }

            const auto state = handles.FinalSnapshot->GetSnapshot();
            const auto &cursor = handles.FinalSnapshot->GetSnapshotCursor();
            std::cout << "captured";
            if (state != nullptr) std::cout << " category=" << state->category();
            std::cout << " step=" << cursor.Step;
            if (cursor.SimulationTime.has_value()) std::cout << " t=" << *cursor.SimulationTime;
            std::cout << " reason=" << Slab::Studios::Common::ToDisplayString(handles.FinalSnapshot->GetSnapshotReason());

            auto kgState = std::dynamic_pointer_cast<const Slab::Models::KGRtoR::FEquationState>(state);
            if (kgState != nullptr) {
                auto *phiBase = &kgState->getPhi();
                auto *phiNumeric = dynamic_cast<Math::RtoR::NumericFunction *>(phiBase);
                if (phiNumeric != nullptr) {
                    try {
                        std::cout << " phi[min,max]=[" << phiNumeric->min() << ", " << phiNumeric->max() << "]";
                    } catch (...) {
                    }
                }
            }
            std::cout << '\n';
        }

        auto PrintDFTSummary(const FRtoRPlaneWavesAnalysisHandlesV2 &handles) -> void {
            if (handles.ScalarDFT == nullptr) return;

            std::cout << "  ScalarDFT";
            if (handles.DFTProbeIndex.has_value()) std::cout << "(probeIndex=" << *handles.DFTProbeIndex << ")";
            std::cout << ": samples=" << handles.ScalarDFT->GetSampleCount();

            const auto magnitudes = handles.ScalarDFT->GetDFTMagnitudes();
            if (magnitudes == nullptr) {
                std::cout << " magnitudes=none\n";
                return;
            }

            const auto &host = magnitudes->getSpace().getHostData(true);
            if (host.size() == 0) {
                std::cout << " magnitudes=empty\n";
                return;
            }

            UInt idx = 0;
            auto peak = host[0];
            for (UInt i = 1; i < host.size(); ++i) {
                if (host[i] > peak) {
                    peak = host[i];
                    idx = i;
                }
            }
            std::cout << " peak=("
                      << "k=" << magnitudes->mapIntToPos(idx)
                      << ", mag=" << peak
                      << ", idx=" << idx
                      << ")\n";
        }

        auto PrintAnalysisSummary(const FRtoRPlaneWavesAnalysisHandlesV2 &handles) -> void {
            if (!handles.HasAny()) return;
            std::cout << "Analysis:\n";
            PrintHistorySummary(handles);
            PrintSnapshotSummary(handles);
            PrintDFTSummary(handles);
        }

        auto BuildRecipeWithOptionalAnalysis(const FRtoRPlaneWavesExecutionConfig &cfg,
                                             const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
                                             FRtoRPlaneWavesAnalysisHandlesV2 &handles)
            -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
            auto recipe = BuildRtoRPlaneWavesRecipeV2(cfg, liveView);
            auto extras = BuildAnalysisSubscriptions(cfg, handles);
            if (extras.empty()) return recipe;
            return New<Math::Numerics::V2::FAppendedSubscriptionsRecipeV2>(recipe, std::move(extras));
        }

    } // namespace

    auto BuildRtoRPlaneWavesRecipeConfigV2(const FRtoRPlaneWavesExecutionConfig &cfg)
        -> Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 {
        Slab::Models::KGRtoR::PlaneWaves::V2::FKGRtoRPlaneWavesConfigV2 recipeCfg;
        recipeCfg.N = cfg.N;
        recipeCfg.L = cfg.L;
        recipeCfg.Dt = cfg.Dt;
        recipeCfg.Steps = cfg.Steps;
        recipeCfg.XCenter = cfg.XCenter;
        recipeCfg.Q = cfg.Q;
        recipeCfg.Harmonic = cfg.Harmonic;
        return recipeCfg;
    }

    auto BuildRtoRPlaneWavesRecipeV2(const FRtoRPlaneWavesExecutionConfig &cfg,
                                     const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView)
        -> TPointer<Math::Numerics::V2::FSimulationRecipeV2> {
        using namespace Slab::Models::KGRtoR::PlaneWaves::V2;

        const auto recipeCfg = BuildRtoRPlaneWavesRecipeConfigV2(cfg);
        if (liveView != nullptr) {
            auto recipe = New<FKGRtoRPlaneWavesRecipeV2>(recipeCfg, cfg.Interval, liveView);
            recipe->SetLiveViewIntervalSteps(cfg.MonitorInterval);
            return recipe;
        }

        return New<FKGRtoRPlaneWavesRecipeV2>(recipeCfg, cfg.Interval);
    }

    auto BuildRtoRPlaneWavesPassiveMonitorWindowV2(
        const FRtoRPlaneWavesExecutionConfig &cfg,
        const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView) -> TPointer<Graphics::FSlabWindow> {
        if (liveView == nullptr) throw Exception("RtoR passive monitor requires a live view.");
        return New<Slab::Studios::Common::Monitors::V2::FRtoRPlaneWavesPassiveMonitorWindowV2>(liveView, cfg.Steps);
    }

    auto RunRtoRPlaneWavesV2(const FRtoRPlaneWavesExecutionConfig &cfg) -> int {
        using namespace Slab::Math::Numerics::V2;

        auto runCfg = cfg;
        FinalizeRtoRPlaneWavesExecutionConfigV2(runCfg);
        FRtoRPlaneWavesAnalysisHandlesV2 analysisHandles;

        if (runCfg.bEnableGLMonitor) {
            auto liveView = New<Math::LiveData::V2::FSessionLiveViewV2>();
            auto recipe = BuildRecipeWithOptionalAnalysis(runCfg, liveView, analysisHandles);
            auto monitor = BuildRtoRPlaneWavesPassiveMonitorWindowV2(runCfg, liveView);
            return Slab::Studios::Common::RunGLFWMonitoredNumericTaskV2(
                "Studios KGRtoR Plane Waves V2 Monitor",
                recipe,
                monitor,
                static_cast<size_t>(runCfg.Batch),
                [&](const FNumericTaskV2 &) { PrintAnalysisSummary(analysisHandles); });
        }

        auto recipe = BuildRecipeWithOptionalAnalysis(runCfg, nullptr, analysisHandles);
        auto task = New<FNumericTaskV2>(recipe, false, static_cast<size_t>(runCfg.Batch));
        const auto status = Slab::Studios::Common::RunTaskAndWait(*task);
        Slab::Studios::Common::PrintNumericTaskSummary(*task);
        PrintAnalysisSummary(analysisHandles);
        return Slab::Studios::Common::ExitCodeFromTaskStatus(status);
    }

} // namespace Slab::Studios::Common::Simulations::V2
