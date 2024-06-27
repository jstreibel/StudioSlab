//
// Created by joao on 16/04/24.
//

#include "RtoRStatisticalMonitor.h"

#include "imgui.h"

#include "Math/Function/RtoR/Operations/Histogram.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Graphics/Graph/PlotThemeManager.h"
#include "Graphics/Graph/Plotter.h"
#include "Utils/Threads.h"

#include <sstream>
#include <array>

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
#define CHECK_GL_ERRORS(count) Graphics::OpenGL::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

// Ok to touch these:
#define HISTOGRAM_SHOULD_BE_PRETTY false
#define UPDATE_HISTORY_EVERY_STEP true
#define linesOffset (3 * params.getL())
#define xMinLinesInFullHistoryView (params.getxMin() - linesOffset)
#define xMaxLinesInFullHistoryView (params.getxMax() + linesOffset)

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    RtoRStatisticsPanel::RtoRStatisticsPanel(const NumericConfig &params, KGEnergy &hamiltonian,
                                             Graphics::GUIWindow &guiWindow)
            : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ statistics panel",
                                  "panel for statistic analysis of simulation data"),
              hamiltonian(hamiltonian) {

        addWindow(Slab::Naked(mCorrelationGraph));

        {
            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles.begin();

            auto mTemperaturesGraph = Slab::New<Graphics::PlottingWindow>("T");

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
            mTemperaturesGraph->getRegion().animate_xMax(params.gett());

            addWindowToColumn(mTemperaturesGraph, 0);

            auto TParam = InterfaceManager::getInstance().getParametersValues({"T"});
            if (!TParam.empty()) {
                auto T = std::stod(TParam[0].second);
                auto pts = Math::Point2DVec({{-.1,                T},
                                             {params.gett() + .1, T}});
                auto Tstyle = (*style++).permuteColors();
                Tstyle.filled = false;
                Graphics::Plotter::AddPointSet(mTemperaturesGraph,
                                               Slab::New<Math::PointSet>(pts), Tstyle, "T (nominal)");
            }
        }

        {
            auto mHistogramsGraphK = Slab::New<Graphics::PlottingWindow>   ("k histogram", MANUAL_REVIEW_GRAPH_LIMITS);
            auto mHistogramsGraphGrad = Slab::New<Graphics::PlottingWindow>("w histogram", MANUAL_REVIEW_GRAPH_LIMITS);
            auto mHistogramsGraphV = Slab::New<Graphics::PlottingWindow>   ("v histogram", MANUAL_REVIEW_GRAPH_LIMITS);
            auto mHistogramsGraphE = Slab::New<Graphics::PlottingWindow>   ("e histogram", MANUAL_REVIEW_GRAPH_LIMITS);


            auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles.begin();
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
            histogramsPanel->addWindow(mHistogramsGraphV);
            histogramsPanel->addWindow(mHistogramsGraphGrad);
            histogramsPanel->addWindow(mHistogramsGraphK);
            histogramsPanel->addWindow(mHistogramsGraphE);

            addWindow(Graphics::Window_ptr(histogramsPanel), true);
        }

        // setColumnRelativeWidth(0, 0.125);
        setColumnRelativeWidth(0, -1);
        setColumnRelativeWidth(1, 0.40);
    }

    void RtoRStatisticsPanel::setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                                   const R2toRFunctionArtist_ptr &simHistoryArtist) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryArtist);

        auto simulationHistoryGraph = Slab::New<PlottingWindow>();
        simulationHistoryGraph->addArtist(simulationHistoryArtist);
        addWindow(simulationHistoryGraph, true, 0.20);
    }



    void RtoRStatisticsPanel::draw() {
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

            guiWindow.begin();
            if (ImGui::CollapsingHeader("Probability density functions")) {
                ImGui::SliderInt("n bins", &nbins, 10, 2000);
                ImGui::Checkbox("Pretty bars", &pretty);
            }
            guiWindow.end();


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

        guiWindow.addVolatileStat("<\\br>");
        for (const auto &p: InterfaceManager::getInstance().getParametersValues({"T", "k", "i"})) {
            auto name = p.first;
            if (name == "i") name = "transient";
            guiWindow.addVolatileStat(name + " = " + p.second);
        }

        RtoRPanel::draw();
    }

    void RtoRStatisticsPanel::drawGUI() {
        guiWindow.begin();

        if (ImGui::CollapsingHeader("Statistical")) {
            auto transient = (float)transientHint;
            if (ImGui::SliderFloat("Transient hint", &transient, .0f, (float) params.gett())) {
                setTransientHint((Real) transient);
            }
        }

        guiWindow.end();
    }

    void RtoRStatisticsPanel::setTransientHint(Real value) {
        transientHint = value;
    }

    void RtoRStatisticsPanel::updateEnergyData() {
        auto L = params.getL();

        fix t = lastPacket.getSimTime();

        auto U = hamiltonian.getTotalEnergy();
        auto K = hamiltonian.getTotalKineticEnergy();
        auto W = hamiltonian.getTotalGradientEnergy();
        auto V = hamiltonian.getTotalPotentialEnergy();

        u = U / L;
        barϕ = V / L;
        tau = 2 * K / L;
        tau_indirect = u - .5 * barϕ;
        fix tau_2 = barϕ + 2 * W / L;

        temperature1HistoryData.addPoint({t, tau});
        temperature2HistoryData.addPoint({t, tau_indirect});
        temperature3HistoryData.addPoint({t, tau_2});

        std::ostringstream ss;
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles.begin();
        guiWindow.addVolatileStat("<\\br>");
        guiWindow.addVolatileStat(Str("U = ") + ToStr(U), (style++)->lineColor);
        guiWindow.addVolatileStat(Str("K = ") + ToStr(K), (style++)->lineColor);
        guiWindow.addVolatileStat(Str("W = ") + ToStr(W), (style++)->lineColor);
        guiWindow.addVolatileStat(Str("V = ") + ToStr(V), (style++)->lineColor);
        guiWindow.addVolatileStat(Str("u = U/L = ") + ToStr(u, 2));

        style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles.begin();
        fix decimalPlaces = 3;
        guiWindow.addVolatileStat(Str("τₖ = <dotϕ^2> = 2K/L = ") + ToStr(tau, decimalPlaces),
                                  (style++)->lineColor.permute());
        guiWindow.addVolatileStat(Str("τ = u - barφ/2 = ") + ToStr(tau_indirect, decimalPlaces),
                                  (style++)->lineColor.permute());
        guiWindow.addVolatileStat(Str("τ₂ = barphi + w = ") + ToStr((barϕ + 2 * W / L), decimalPlaces),
                                  (style++)->lineColor.permute());
        // guiWindow.addVolatileStat(Str("(τₖ+τ₂)/2 = ") + ToStr((tau_avg), decimalPlaces),  (style++)->lineColor);
    }


}