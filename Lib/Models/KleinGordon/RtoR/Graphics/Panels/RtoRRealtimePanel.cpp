//
// Created by joao on 18/04/24.
//

#include "RtoRRealtimePanel.h"

#include <utility>

#include "3rdParty/ImGui.h"
#include "Models/KleinGordon/KG-Solver.h"
#include "Graphics/Plot2D/Plotter.h"

// Don't touch these:
#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))

#define ADD_NEW_COLUMN true
#define MANUAL_REVIEW_GRAPH_LIMITS false
#define FIRST_TIMER(code)           \
    static bool firstTimer = true;  \
    if (firstTimer) {               \
        firstTimer = false;         \
        code                        \
    }
#define CHECK_GL_ERRORS(count) Graphics::OpenGL::CheckGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

// Ok to touch these:
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true
#define GOOD_ENOUGH_NUMBER_OF_SAMPLES 300
#define MAX_SAMPLES 50000
#define linesOffset (3 * params.getL())


namespace Slab::Models::KGRtoR {

    FRealtimePanel::FRealtimePanel(
        const Pointer<KGNumericConfig> &Params,
        KGEnergy &Hamiltonian,
        FGUIWindow &GUIWindow)
    : FRtoRPanel(
        Params,
        GUIWindow,
        Hamiltonian,
        "ℝ↦ℝ realtime monitor",
        "realtime monitoring of simulation state")
    , FieldsGraph("Fields", GUIWindow.GetGUIWindowContext())
    , EnergyGraph("Energy", GUIWindow.GetGUIWindowContext())
    // , imGuiWindow(Naked(mEnergyGraph))
    {
        {
            const auto CurrStyle = PlotThemeManager::GetCurrent();

            auto sty = CurrStyle->FuncPlotStyles.begin();

            Plotter::AddPointSet(Naked(EnergyGraph),
                                           Naked(UHistoryData),
                                           *sty, "U/L");
            Plotter::AddPointSet(Naked(EnergyGraph),
                                           Naked(KHistoryData),
                                           *++sty, "K/L");
            Plotter::AddPointSet(Naked(EnergyGraph),
                                           Naked(WHistoryData),
                                           *++sty, "<(𝜕ₓϕ)²>/2=∫(𝜕ₓϕ)²dx/2L");
            Plotter::AddPointSet(Naked(EnergyGraph),
                                           Naked(VHistoryData),
                                           *++sty, "<V(ϕ)>=∫V(ϕ)dx/L");

            EnergyGraph.SetAutoReviewGraphRanges(false);
            EnergyGraph.GetAxisArtist().SetHorizontalAxisLabel("t");
            EnergyGraph.GetAxisArtist().SetHorizontalAxisLabel("e");
        }

        {
            fix V_str = Hamiltonian.GetThePotential()->Symbol();
            vArtist = Plotter::AddRtoRFunction(Naked(FieldsGraph),
                                                         Hamiltonian.getPotentialDensity(), V_style,
                                                         Str("V(ϕ)=") + V_str, 1024);
            kArtist = Plotter::AddRtoRFunction(Naked(FieldsGraph),
                                                         Hamiltonian.getKineticDensity(), K_style, "K/L", 1024);
            wArtist = Plotter::AddRtoRFunction(Naked(FieldsGraph),
                                                         Hamiltonian.getGradientDensity(), W_style, "(𝜕ₓϕ)²", 1024);
            uArtist = Plotter::AddRtoRFunction(Naked(FieldsGraph),
                                                         Hamiltonian.getEnergyDensity(), U_style, "E/L", 1024);
        }

        AddWindow(Naked(FieldsGraph));
        AddWindow(Naked(EnergyGraph));

        SetColumnRelativeWidth(0, 0.4);
        SetColumnRelativeWidth(1, -1);
    }

    void FRealtimePanel::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        UpdateEnergyData();

        fix V_str = Hamiltonian.GetThePotential()->Symbol();

        int ErrorCount = 0;

        CHECK_GL_ERRORS(ErrorCount++)

        // imGuiWindow.draw();

        // ************************ RT MONITOR**********************************
        guiWindow.AddExternalDraw([this, V_str]() {
            if (ImGui::CollapsingHeader("Real-time monitor")) {

                if (ImGui::Checkbox(("Show V(ϕ)=" + V_str).c_str(), &bShowPot)) { vArtist->SetVisibility(bShowPot); }
                if (ImGui::Checkbox("Show (𝜕ₜϕ)²/2", &bShowKineticEnergy)) { kArtist->SetVisibility(bShowKineticEnergy); }
                if (ImGui::Checkbox("Show (𝜕ₓϕ)²/2", &bShowGradientEnergy)) {
                    wArtist->SetVisibility(bShowGradientEnergy);
                }
                if (ImGui::Checkbox("Show e", &bShowEnergyDensity)) { uArtist->SetVisibility(bShowEnergyDensity); }
            }
        });

        CHECK_GL_ERRORS(ErrorCount++)

        FRtoRPanel::ImmediateDraw(PlatformWindow);
    }

    void FRealtimePanel::SetSimulationHistory(
        Pointer<const R2toR::FNumericFunction> SimHistory,
        const R2toRFunctionArtist_ptr &SimHistoryGraph)
    {
        FRtoRPanel::SetSimulationHistory(SimHistory, SimHistoryGraph);

        auto simulationHistoryGraph = Slab::New<Graphics::FPlot2DWindow>("Simulation history", guiWindow.GetGUIWindowContext());
        simulationHistoryGraph->AddArtist(SimHistoryGraph);

        AddWindow(simulationHistoryGraph, ADD_NEW_COLUMN);
        SetColumnRelativeWidth(1, -1);
    }

    void FRealtimePanel::SetSpaceFourierHistory(
        Pointer<const R2toR::FNumericFunction> SftHistory,
        const FDFTDataHistory &DFTData,
        const R2toRFunctionArtist_ptr &SftHistoryArtist)
    {
        FRtoRPanel::SetSpaceFourierHistory(SftHistory, DFTData, SftHistoryArtist);

        auto sftHistoryGraph = Slab::New<FPlot2DWindow>("Space Fourier transform", guiWindow.GetGUIWindowContext());
        sftHistoryGraph->AddArtist(SftHistoryArtist);

        AddWindow(sftHistoryGraph);
    }

    void FRealtimePanel::UpdateEnergyData() {
        const auto t = static_cast<DevFloat>(LastPacket.GetSteps()) * Params->Getdt();

        auto U = Hamiltonian.GetTotalEnergy();
        auto K = Hamiltonian.GetTotalKineticEnergy();
        auto W = Hamiltonian.GetTotalGradientEnergy();
        auto V = Hamiltonian.GetTotalPotentialEnergy();

        auto Factor = 1.0 / Params->GetL();

        UHistoryData.AddPoint({t, U * Factor});
        KHistoryData.AddPoint({t, K * Factor});
        WHistoryData.AddPoint({t, W * Factor});
        VHistoryData.AddPoint({t, V * Factor});

        auto xMax = t;

        auto yMin = min(UHistoryData.getMin().y,
                        min(KHistoryData.getMin().y, min(WHistoryData.getMin().y, VHistoryData.getMin().y)));
        auto yMax = max(UHistoryData.getMax().y,
                        min(KHistoryData.getMax().y, min(WHistoryData.getMax().y, VHistoryData.getMax().y)));

        EnergyGraph.GetRegion().animate_yMax(yMax);
        EnergyGraph.GetRegion().animate_yMin(yMin);

        EnergyGraph.GetRegion().animate_xMax(xMax);
    }

}