//
// Created by joao on 16/04/24.
//

#include "RtoRStatisticalMonitor.h"

#include "3rdParty/ImGui.h"

#include "Math/Function/RtoR/Operations/Histogram.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Utils/Threads.h"

#include <sstream>
#include <array>

// Don't touch these:
#define max(a, b) ((a)>(b)?(a):(b))
#define min(a, b) ((a)<(b)?(a):(b))
#define ADD_NEW_COLUMN true
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
#define linesOffset (3 * params.getL())
#define xMinLinesInFullHistoryView (params.getxMin() - linesOffset)
#define xMaxLinesInFullHistoryView (params.getxMax() + linesOffset)

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    RtoRStatisticsPanel::RtoRStatisticsPanel(const Pointer<KGNumericConfig> &params, KGEnergy &hamiltonian,
                                             Graphics::FGUIWindow &guiWindow)
            : FRtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ statistics panel", "panel for statistic analysis of simulation data")
            , hamiltonian(hamiltonian)
            , mCorrelationGraph("Correlations"){

        AddWindow(Slab::Naked(mCorrelationGraph));

        {
            auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles.begin();

            auto mTemperaturesGraph = Slab::New<Graphics::Plot2DWindow>("T");

            Graphics::Plotter::AddPointSet(mTemperaturesGraph,
                                           Slab::Naked(temperature1HistoryData),
                                           (*style++).permuteColors(), "τₖ=2<K>/L");
            Graphics::Plotter::AddPointSet(mTemperaturesGraph,
                                           Slab::Naked(temperature2HistoryData),
                                           (*style++).permuteColors(), "τ");
            Graphics::Plotter::AddPointSet(mTemperaturesGraph,
                                           Slab::Naked(temperature3HistoryData),
                                           (*style++).permuteColors(), "τ₂");
            // mTemperaturesGraph.addPointSet(DummyPtr(temperature4HistoryData), (*style++), "(τₖ+τ₂)/2");
            mTemperaturesGraph->getRegion().animate_xMax(params->gett());

            addWindowToColumn(mTemperaturesGraph, 0);

            auto TParam = FCommandLineInterfaceManager::getInstance().getParametersValues({"T"});
            if (!TParam.empty()) {
                auto T = std::stod(TParam[0].second);
                auto pts = Math::Point2DVec({{-.1,                T},
                                             {params->gett() + .1, T}});
                auto Tstyle = (*style++).permuteColors();
                Tstyle.filled = false;
                Graphics::Plotter::AddPointSet(mTemperaturesGraph,
                                               Slab::New<Math::PointSet>(pts), Tstyle, "T (nominal)");
            }
        }

        {
            auto mHistogramsGraphK = Slab::New<Graphics::Plot2DWindow>   ("k histogram");
            auto mHistogramsGraphGrad = Slab::New<Graphics::Plot2DWindow>("w histogram");
            auto mHistogramsGraphV = Slab::New<Graphics::Plot2DWindow>   ("v histogram");
            auto mHistogramsGraphE = Slab::New<Graphics::Plot2DWindow>   ("e histogram");


            auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles.begin();
            Graphics::Plotter::AddPointSet(mHistogramsGraphE,
                                           Slab::Naked(histogramEData),
                                           *style++, "E");
            Graphics::Plotter::AddPointSet(mHistogramsGraphK,
                                           Slab::Naked(histogramKData),
                                           *style++, "K");
            Graphics::Plotter::AddPointSet(mHistogramsGraphGrad,
                                           Slab::Naked(histogramGradData),
                                           *style++, "grad");
            Graphics::Plotter::AddPointSet(mHistogramsGraphV,
                                           Slab::Naked(histogramVData),
                                           *style++, "V");

            auto *histogramsPanel = new Graphics::WindowPanel();
            histogramsPanel->AddWindow(mHistogramsGraphV);
            histogramsPanel->AddWindow(mHistogramsGraphGrad);
            histogramsPanel->AddWindow(mHistogramsGraphK);
            histogramsPanel->AddWindow(mHistogramsGraphE);

            AddWindow(Pointer<Graphics::FSlabWindow>(histogramsPanel), true);
        }

        // setColumnRelativeWidth(0, 0.125);
        SetColumnRelativeWidth(0, -1);
        SetColumnRelativeWidth(1, 0.40);
    }

    void RtoRStatisticsPanel::SetSimulationHistory(Pointer<const R2toR::FNumericFunction>
 simulationHistory,
                                                   const R2toRFunctionArtist_ptr &simHistoryArtist) {
        FRtoRPanel::SetSimulationHistory(simulationHistory, simHistoryArtist);

        auto simulationHistoryGraph = Slab::New<Plot2DWindow>("Simulation history");
        simulationHistoryGraph->addArtist(simulationHistoryArtist);
        AddWindow(simulationHistoryGraph, true, 0.20);
    }



    void RtoRStatisticsPanel::Draw() {
        int errorCount = 0;
        CHECK_GL_ERRORS(errorCount++)

        drawGUI();

        CHECK_GL_ERRORS(errorCount++)

        updateEnergyData();

        CHECK_GL_ERRORS(errorCount++)

        // *************************** Histograms *****************************
        {
            RtoR::Histogram histogram;
            static auto nbins = 200;
            static auto pretty = HISTOGRAM_SHOULD_BE_PRETTY;

            guiWindow.AddExternalDraw([]() {
                if (ImGui::CollapsingHeader("Probability density functions")) {
                    ImGui::SliderInt("n bins", &nbins, 10, 2000);
                    ImGui::Checkbox("Pretty bars", &pretty);
                }
            });

            histogram.Compute(hamiltonian.getKineticDensity(), nbins);
            histogram.renderPDFToPointSet(Naked(histogramKData), pretty);

            histogram.Compute(hamiltonian.getGradientDensity(), nbins);
            histogram.renderPDFToPointSet(Naked(histogramGradData), pretty);

            histogram.Compute(hamiltonian.getPotentialDensity(), nbins);
            histogram.renderPDFToPointSet(Naked(histogramVData), pretty);

            histogram.Compute(hamiltonian.getEnergyDensity(), nbins);
            histogram.renderPDFToPointSet(Naked(histogramEData), pretty);
        }

        // *************************** MY BEAUTY *****************************

        guiWindow.AddVolatileStat("<\\br>");
        for (const auto &p: FCommandLineInterfaceManager::getInstance().getParametersValues({"T", "k", "i"})) {
            auto name = p.first;
            if (name == "i") name = "transient";
            guiWindow.AddVolatileStat(name + " = " + p.second);
        }

        FRtoRPanel::Draw();
    }

    void RtoRStatisticsPanel::drawGUI() {
        guiWindow.AddExternalDraw([this]() {
            if (ImGui::CollapsingHeader("Statistical")) {
                auto transient = (float) transientHint;
                if (ImGui::SliderFloat("Transient hint", &transient, .0f, (float) Params->gett())) {
                    SetTransientHint((DevFloat) transient);
                }
            }
        });
    }

    void RtoRStatisticsPanel::SetTransientHint(DevFloat value) {
        transientHint = value;
    }

    void RtoRStatisticsPanel::updateEnergyData() {
        auto L = Params->GetL();

        fix t = LastPacket.GetSteps()*Params->Getdt();

        auto U = hamiltonian.GetTotalEnergy();
        auto K = hamiltonian.GetTotalKineticEnergy();
        auto W = hamiltonian.GetTotalGradientEnergy();
        auto V = hamiltonian.GetTotalPotentialEnergy();

        u = U / L;
        barϕ = V / L;
        tau = 2 * K / L;
        tau_indirect = u - .5 * barϕ;
        fix tau_2 = barϕ + 2 * W / L;

        temperature1HistoryData.AddPoint({t, tau});
        temperature2HistoryData.AddPoint({t, tau_indirect});
        temperature3HistoryData.AddPoint({t, tau_2});

        std::ostringstream ss;
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles.begin();
        guiWindow.AddVolatileStat("<\\br>");
        guiWindow.AddVolatileStat(Str("U = ") + ToStr(U), (style++)->lineColor);
        guiWindow.AddVolatileStat(Str("K = ") + ToStr(K), (style++)->lineColor);
        guiWindow.AddVolatileStat(Str("W = ") + ToStr(W), (style++)->lineColor);
        guiWindow.AddVolatileStat(Str("V = ") + ToStr(V), (style++)->lineColor);
        guiWindow.AddVolatileStat(Str("u = U/L = ") + ToStr(u, 2));

        style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles.begin();
        fix decimalPlaces = 3;
        guiWindow.AddVolatileStat(Str("τₖ = <dotϕ^2> = 2K/L = ") + ToStr(tau, decimalPlaces),
                                  (style++)->lineColor.permute());
        guiWindow.AddVolatileStat(Str("τ = u - barφ/2 = ") + ToStr(tau_indirect, decimalPlaces),
                                  (style++)->lineColor.permute());
        guiWindow.AddVolatileStat(Str("τ₂ = barphi + w = ") + ToStr((barϕ + 2 * W / L), decimalPlaces),
                                  (style++)->lineColor.permute());
        // guiWindow.addVolatileStat(Str("(τₖ+τ₂)/2 = ") + ToStr((tau_avg), decimalPlaces),  (style++)->lineColor);
    }


}