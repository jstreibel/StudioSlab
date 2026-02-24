#include "SPIPassiveMonitorWindowV2.h"

#include "Graphics/Plot2D/PlotThemeManager.h"

#include "Math/Function/RtoR2/StraightLine.h"

#include "Models/Stochastic-Path-Integral/SPI-State.h"

#include "../../SessionLiveViewStatsV2.h"

namespace Slab::Studios::Common::Monitors::V2 {

    auto FSPIPassiveMonitorWindowV2::EnsureSectionConfigured(const TPointer<Math::R2toR::FNumericFunction> &phi) -> void {
        if (phi == nullptr) return;
        if (!SectionArtist.getSections().empty()) return;

        const auto yMin = phi->getDomain().yMin;
        const auto yMax = phi->getDomain().yMax;

        auto line = New<Math::RtoR2::StraightLine>(Math::Real2D{0, yMin}, Math::Real2D{0, yMax}, yMin, yMax);
        auto style = Graphics::FPlotThemeManager::GetCurrent()->FuncPlotStyles[0].clone();
        SectionArtist.addSection(line, style, "x=0 section");
    }

    auto FSPIPassiveMonitorWindowV2::UpdateStatsWindow() -> void {
        Slab::Studios::Common::AppendSessionLiveViewStats(
            GuiWindow,
            "SPI V2 passive monitor",
            LastTelemetry,
            LiveView->HasBoundSession(),
            bLastLeaseAcquired,
            MaxSteps);
    }

    FSPIPassiveMonitorWindowV2::FSPIPassiveMonitorWindowV2(
            const TPointer<Math::LiveData::V2::FSessionLiveViewV2> &liveView,
            const UIntBig maxSteps)
    : FWindowPanel(Graphics::FSlabWindowConfig("SPI V2 GL Monitor"))
    , LiveView(liveView)
    , GuiWindow(New<Graphics::FGUIWindow>(Graphics::FSlabWindowConfig("SPI V2 Telemetry")))
    , SectionWindow("SPI field section")
    , MaxSteps(maxSteps) {
        if (LiveView == nullptr) throw Exception("SPI passive monitor requires a live view.");

        SectionArtist.SetAffectGraphRanges(true);
        AddWindow(GuiWindow, false, 0.25f);
        AddWindow(Naked(SectionWindow), true, 0.75f);
        SetColumnRelativeWidth(0, 0.25f);

        SectionWindow.AddArtist(Naked(SectionArtist));
        SectionWindow.SetAutoReviewGraphRanges(true);
    }

    auto FSPIPassiveMonitorWindowV2::ImmediateDraw(const Graphics::FPlatformWindow &platformWindow) -> void {
        auto telemetry = LiveView->TryGetTelemetry();
        if (telemetry.has_value()) LastTelemetry = telemetry;

        // For local GL monitoring, prefer a coherent lease over best-effort skipping.
        auto leaseOpt = LiveView->AcquireReadLease();
        bLastLeaseAcquired = leaseOpt.has_value();

        if (leaseOpt.has_value()) {
            auto spiState = std::dynamic_pointer_cast<const Models::StochasticPathIntegrals::SPIState>(leaseOpt->GetState());
            if (spiState != nullptr) {
                auto phi = spiState->getPhi();
                EnsureSectionConfigured(phi);
                SectionArtist.setFunction(phi);
            } else {
                SectionArtist.setFunction(nullptr);
            }
        } else {
            // Avoid rendering through a stale pointer after lease release or session invalidation.
            SectionArtist.setFunction(nullptr);
        }

        UpdateStatsWindow();
        FWindowPanel::ImmediateDraw(platformWindow);
    }

} // namespace Slab::Studios::Common::Monitors::V2
