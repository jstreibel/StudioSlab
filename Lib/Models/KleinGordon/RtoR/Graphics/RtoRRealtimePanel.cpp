//
// Created by joao on 18/04/24.
//

#include "RtoRRealtimePanel.h"

#include <utility>

#include "3rdParty/imgui/imgui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Math/Function/RtoR/Calc/DiscreteFourierTransform.h"
#include "Graphics/Graph/Plotter.h"
// #include "Math/Function/R2toR/DFT.h"

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
#define GOOD_ENOUGH_NUMBER_OF_SAMPLES 300
#define MAX_SAMPLES 50000
#define linesOffset (3 * params.getL())


namespace Slab::Models::KGRtoR {

    RealtimePanel::RealtimePanel(const NumericConfig &params, KGEnergy &hamiltonian,
                                       Graphics::GUIWindow &guiWindow)
            : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ realtime monitor",
                                  "realtime monitoring of simulation state")
            , mFieldsGraph(params.getxMin(), params.getxMax(), -1, 1, "Fields")
            , mEnergyGraph("Energy") {
        auto currStyle = Graphics::PlotThemeManager::GetCurrent();

        {
            auto sty = currStyle->funcPlotStyles.begin();

            Graphics::Plotter::AddPointSet(Slab::DummyPointer(mEnergyGraph),
                                           Slab::DummyPointer(UHistoryData),
                                           *sty, "U/L");
            Graphics::Plotter::AddPointSet(Slab::DummyPointer(mEnergyGraph),
                                           Slab::DummyPointer(KHistoryData),
                                           *++sty, "K/L");
            Graphics::Plotter::AddPointSet(Slab::DummyPointer(mEnergyGraph),
                                           Slab::DummyPointer(WHistoryData),
                                           *++sty, "<(𝜕ₓϕ)²>/2=∫(𝜕ₓϕ)²dx/2L");
            Graphics::Plotter::AddPointSet(Slab::DummyPointer(mEnergyGraph),
                                           Slab::DummyPointer(VHistoryData),
                                           *++sty, "<V(ϕ)>=∫V(ϕ)dx/L");

            mEnergyGraph.setAutoReviewGraphRanges(false);
            mEnergyGraph.getAxisArtist().setHorizontalAxisLabel("t");
            mEnergyGraph.getAxisArtist().setHorizontalAxisLabel("e");
        }

        {
            fix V_str = hamiltonian.getThePotential()->symbol();
            vArtist = Graphics::Plotter::AddRtoRFunction(Slab::DummyPointer(mFieldsGraph),
                                                         hamiltonian.getPotentialDensity(), V_style,
                                                         Str("V(ϕ)=") + V_str, 1024);
            kArtist = Graphics::Plotter::AddRtoRFunction(Slab::DummyPointer(mFieldsGraph),
                                                         hamiltonian.getKineticDensity(), K_style, "K/L", 1024);
            wArtist = Graphics::Plotter::AddRtoRFunction(Slab::DummyPointer(mFieldsGraph),
                                                         hamiltonian.getGradientDensity(), W_style, "(𝜕ₓϕ)²", 1024);
            uArtist = Graphics::Plotter::AddRtoRFunction(Slab::DummyPointer(mFieldsGraph),
                                                         hamiltonian.getEnergyDensity(), U_style, "E/L", 1024);
        }

        addWindow(Slab::DummyPointer(mFieldsGraph));
        addWindow(Slab::DummyPointer(mEnergyGraph));

        setColumnRelativeWidth(0, 0.4);
        setColumnRelativeWidth(1, -1);
    }

    void RealtimePanel::draw() {
        updateEnergyData();

        fix V_str = hamiltonian.getThePotential()->symbol();

        int errorCount = 0;

        CHECK_GL_ERRORS(errorCount++)

        // ************************ RT MONITOR**********************************
        guiWindow.begin();
        if (ImGui::CollapsingHeader("Real-time monitor")) {

            if (ImGui::Checkbox(("Show V(ϕ)=" + V_str).c_str(), &showPot)) { vArtist->setVisibility(showPot); }
            if (ImGui::Checkbox("Show (𝜕ₜϕ)²/2", &showKineticEnergy)) { kArtist->setVisibility(showKineticEnergy); }
            if (ImGui::Checkbox("Show (𝜕ₓϕ)²/2", &showGradientEnergy)) { wArtist->setVisibility(showGradientEnergy); }
            if (ImGui::Checkbox("Show e", &showEnergyDensity)) { uArtist->setVisibility(showEnergyDensity); }
        }
        guiWindow.end();

        CHECK_GL_ERRORS(errorCount++)

        RtoRPanel::draw();
    }

    void RealtimePanel::setSimulationHistory(R2toR::DiscreteFunction_constptr simHistory,
                                             const Graphics::R2toRFunctionArtist_ptr &simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simHistory, simHistoryGraph);

        auto simulationHistoryGraph = Slab::New<Graphics::PlottingWindow>();
        simulationHistoryGraph->addArtist(simHistoryGraph);

        addWindow(simulationHistoryGraph, ADD_NEW_COLUMN);
        setColumnRelativeWidth(1, -1);
    }

    void RealtimePanel::setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                               const DFTDataHistory &dftData,
                                               const Graphics::R2toRFunctionArtist_ptr &sftHistoryArtist) {
        RtoRPanel::setSpaceFourierHistory(sftHistory, dftData, sftHistoryArtist);

        auto sftHistoryGraph = Slab::New<PlottingWindow>();
        sftHistoryGraph->addArtist(sftHistoryArtist);

        addWindow(sftHistoryGraph);
    }

    void RealtimePanel::updateEnergyData() {
        const auto t = lastPacket.getSimTime();

        auto U = hamiltonian.getTotalEnergy();
        auto K = hamiltonian.getTotalKineticEnergy();
        auto W = hamiltonian.getTotalGradientEnergy();
        auto V = hamiltonian.getTotalPotentialEnergy();

        auto factor = 1.0 / params.getL();

        UHistoryData.addPoint({t, U * factor});
        KHistoryData.addPoint({t, K * factor});
        WHistoryData.addPoint({t, W * factor});
        VHistoryData.addPoint({t, V * factor});

        auto xMax = t;

        auto yMin = min(UHistoryData.getMin().y,
                        min(KHistoryData.getMin().y, min(WHistoryData.getMin().y, VHistoryData.getMin().y)));
        auto yMax = max(UHistoryData.getMax().y,
                        min(KHistoryData.getMax().y, min(WHistoryData.getMax().y, VHistoryData.getMax().y)));

        mEnergyGraph.set_yMax(yMax);
        mEnergyGraph.set_yMin(yMin);

        mEnergyGraph.set_xMax(xMax);
    }

}