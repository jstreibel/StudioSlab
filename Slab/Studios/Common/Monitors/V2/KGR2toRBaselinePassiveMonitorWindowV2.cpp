#include "KGR2toRBaselinePassiveMonitorWindowV2.h"

#include "3rdParty/ImGui.h"

#include "Models/KleinGordon/R2toR/EquationState.h"

#include "../../SessionLiveViewStatsV2.h"
#include "../../Simulations/V2/KGR2toRControlTopicsV2.h"

#include <algorithm>
#include <chrono>

namespace Slab::Studios::Common::Monitors::V2 {

    auto FR2toRBaselinePassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        std::optional<Str> extraLine = Str("Display mode: copied phi");
        const bool bHasBoundSession = LastStatus.has_value()
            ? LastStatus->bHasBoundSession
            : (SessionTopic != nullptr && SessionTopic->HasBoundSession());
        if (LastStatus.has_value()) {
            extraLine = *extraLine + " | Run: " + Slab::Studios::Common::ToDisplayString(LastStatus->RunState);
        }
        if (bEnableControlPublisher) {
            extraLine = *extraLine + " | LiveControl: on";
        }

        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "KGR2toR Baseline V2 passive monitor",
            LastTelemetry,
            bHasBoundSession,
            bLastLeaseAcquired,
            MaxSteps,
            extraLine);
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::SetPlotFromState(
            const TPointer<const Math::Base::EquationState> &state) -> void {
        if (FieldArtist == nullptr) return;

        auto kgState = std::dynamic_pointer_cast<const Math::R2toR::EquationState>(state);
        if (kgState == nullptr) {
            FieldArtist->setFunction(nullptr);
            return;
        }

        auto *phiBase = &kgState->getPhi();
        auto *phiNumeric = dynamic_cast<Math::R2toR::FNumericFunction *>(phiBase);
        if (phiNumeric == nullptr) {
            FieldArtist->setFunction(nullptr);
            return;
        }

        const auto domain = phiNumeric->getDomain();
        const bool bGeometryChanged =
            DisplayPhi == nullptr ||
            DisplayPhi->getN() != phiNumeric->getN() ||
            DisplayPhi->getM() != phiNumeric->getM() ||
            !(DisplayPhi->getDomain() == domain);

        if (bGeometryChanged) {
            const auto xRes = phiNumeric->getN();
            const auto yRes = phiNumeric->getM();
            const auto hx = domain.getLx() / static_cast<DevFloat>(xRes);
            const auto hy = domain.getLy() / static_cast<DevFloat>(yRes);
            DisplayPhi = New<Math::R2toR::NumericFunction_CPU>(xRes, yRes, domain.xMin, domain.yMin, hx, hy);
            FieldArtist->setFunction(DisplayPhi);
            FieldWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }

        phiNumeric->getSpace().syncHost();
        DisplayPhi->getSpace().setToValue(phiNumeric->getSpace());
        FieldArtist->updateMinMax(true);

        if (!bPlotRegionInitialized) {
            FieldWindow.GetRegion().setLimits(domain.xMin, domain.xMax, domain.yMin, domain.yMax);
            bPlotRegionInitialized = true;
        }
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::PublishControlSource() -> void {
        if (!bEnableControlPublisher || !bPublishControlSource || ControlHub == nullptr) return;

        using namespace Slab::Math::LiveControl::V2;
        using namespace Slab::Studios::Common::Simulations::V2;

        const auto now = std::chrono::duration<DevFloat>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        FControlTimestampV2 stamp;
        stamp.Domain = EControlTimeDomainV2::WallClockTime;
        stamp.WallClockSeconds = now;

        FControlSampleV2 centerSample;
        centerSample.Value = Slab::Math::Real2D{ControlXCenter, ControlYCenter};
        centerSample.Semantic = EControlSemanticV2::Level;
        centerSample.Timestamp = stamp;
        const auto topics = BuildKG2DForcingTopicNamesV2(ControlTopicPrefix);
        ControlHub->GetOrCreateTopic(topics.Center)->Publish(centerSample);

        FControlSampleV2 amplitudeSample;
        amplitudeSample.Value = ControlAmplitude;
        amplitudeSample.Semantic = EControlSemanticV2::Level;
        amplitudeSample.Timestamp = stamp;
        ControlHub->GetOrCreateTopic(topics.Amplitude)->Publish(amplitudeSample);

        FControlSampleV2 widthSample;
        widthSample.Value = std::max<DevFloat>(ControlWidth, 1e-9);
        widthSample.Semantic = EControlSemanticV2::Level;
        widthSample.Timestamp = stamp;
        ControlHub->GetOrCreateTopic(topics.Width)->Publish(widthSample);

        FControlSampleV2 enabledSample;
        enabledSample.Value = bControlEnabled;
        enabledSample.Semantic = EControlSemanticV2::Level;
        enabledSample.Timestamp = stamp;
        ControlHub->GetOrCreateTopic(topics.Enabled)->Publish(enabledSample);
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::DrawControlWindow() -> void {
        if (!bEnableControlPublisher) return;

        bool bOpen = true;
        if (!ImGui::Begin("KGR2toR Live Control", &bOpen)) {
            ImGui::End();
            return;
        }

        ImGui::TextDisabled("Topic prefix: %s", ControlTopicPrefix.c_str());
        ImGui::Checkbox("Publish controls", &bPublishControlSource);

        ImGui::DragScalar("forcing x", ImGuiDataType_Double, &ControlXCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("forcing y", ImGuiDataType_Double, &ControlYCenter, 0.01, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("forcing width", ImGuiDataType_Double, &ControlWidth, 0.001, nullptr, nullptr, "%.6g");
        ImGui::DragScalar("forcing amplitude", ImGuiDataType_Double, &ControlAmplitude, 0.01, nullptr, nullptr, "%.6g");
        ImGui::Checkbox("forcing enabled", &bControlEnabled);

        ImGui::End();

        if (!bOpen) bEnableControlPublisher = false;
        PublishControlSource();
    }

    FR2toRBaselinePassiveMonitorWindowV2::FR2toRBaselinePassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps,
            const TPointer<Math::LiveControl::V2::FLiveControlHubV2> &controlHub,
            const Str &controlTopicPrefix,
            const bool bEnableControlPublisher,
            const DevFloat controlXCenter,
            const DevFloat controlYCenter,
            const DevFloat controlWidth,
            const DevFloat controlAmplitude,
            const bool bControlEnabled)
    : FWindowPanel(Graphics::FSlabWindowConfig("KGR2toR Baseline V2 GL Monitor"))
    , LiveView(liveView)
    , SessionTopic(liveView != nullptr ? liveView->GetSessionTopic() : nullptr)
    , TelemetryTopic(liveView != nullptr ? liveView->GetTelemetryTopic() : nullptr)
    , StatusTopic(liveView != nullptr ? liveView->GetStatusTopic() : nullptr)
    , ControlHub(controlHub)
    , ControlTopicPrefix(controlTopicPrefix)
    , bEnableControlPublisher(bEnableControlPublisher)
    , ControlXCenter(controlXCenter)
    , ControlYCenter(controlYCenter)
    , ControlWidth(controlWidth)
    , ControlAmplitude(controlAmplitude)
    , bControlEnabled(bControlEnabled)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("KGR2toR Telemetry")))
    , FieldWindow("KGR2toR phi(x,y)")
    , FieldArtist(New<Graphics::R2toRFunctionArtist>())
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("KGR2toR passive monitor requires a live view.");
        if (SessionTopic == nullptr || TelemetryTopic == nullptr) {
            throw Exception("KGR2toR passive monitor requires session/telemetry topics.");
        }

        FieldArtist->SetAffectGraphRanges(false);
        FieldArtist->setDataMutable(true);

        AddWindow(Naked(FieldWindow));

        FieldWindow.AddArtist(FieldArtist);
        FieldWindow.SetAutoReviewGraphRanges(false);
        FieldWindow.GetRegion().setLimits(-1.0, 1.0, -1.0, 1.0);
    }

    auto FR2toRBaselinePassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        const auto telemetry = TelemetryTopic->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;
        if (StatusTopic != nullptr) {
            const auto status = StatusTopic->TryGetStatus();
            if (status.has_value()) LastStatus = status;
        }

        auto leaseOpt = SessionTopic->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();

        if (leaseOpt.has_value()) SetPlotFromState(leaseOpt->GetState());

        UpdateStatsWindow();
        DrawControlWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
