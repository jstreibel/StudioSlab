//
// Created by joao on 18/04/24.
//

#include "RtoRRealtimePanel.h"

#include <utility>

#include "3rdParty/ImGui.h"
#include "Core/Tools/Log.h"
#include "Models/KleinGordon/KG-Solver.h"
#include "Math/Function/RtoR/Operations/DiscreteFourierTransform.h"
#include "Graphics/Plot2D/Plotter.h"
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

    RealtimePanel::RealtimePanel(const Pointer<KGNumericConfig> &params, KGEnergy &hamiltonian,
                                       Graphics::GUIWindow &guiWindow)
            : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ realtime monitor",
                                  "realtime monitoring of simulation state")
            , mFieldsGraph("Fields", guiWindow.GetGUIContext())
            , mEnergyGraph("Energy", guiWindow.GetGUIContext())
            , imGuiWindow(Naked(mEnergyGraph)) {
        auto currStyle = Graphics::PlotThemeManager::GetCurrent();

        {
            auto sty = currStyle->funcPlotStyles.begin();

            Graphics::Plotter::AddPointSet(Slab::Naked(mEnergyGraph),
                                           Slab::Naked(UHistoryData),
                                           *sty, "U/L");
            Graphics::Plotter::AddPointSet(Slab::Naked(mEnergyGraph),
                                           Slab::Naked(KHistoryData),
                                           *++sty, "K/L");
            Graphics::Plotter::AddPointSet(Slab::Naked(mEnergyGraph),
                                           Slab::Naked(WHistoryData),
                                           *++sty, "<(𝜕ₓϕ)²>/2=∫(𝜕ₓϕ)²dx/2L");
            Graphics::Plotter::AddPointSet(Slab::Naked(mEnergyGraph),
                                           Slab::Naked(VHistoryData),
                                           *++sty, "<V(ϕ)>=∫V(ϕ)dx/L");

            mEnergyGraph.setAutoReviewGraphRanges(false);
            mEnergyGraph.getAxisArtist().setHorizontalAxisLabel("t");
            mEnergyGraph.getAxisArtist().setHorizontalAxisLabel("e");
        }

        {
            fix V_str = hamiltonian.getThePotential()->symbol();
            vArtist = Graphics::Plotter::AddRtoRFunction(Slab::Naked(mFieldsGraph),
                                                         hamiltonian.getPotentialDensity(), V_style,
                                                         Str("V(ϕ)=") + V_str, 1024);
            kArtist = Graphics::Plotter::AddRtoRFunction(Slab::Naked(mFieldsGraph),
                                                         hamiltonian.getKineticDensity(), K_style, "K/L", 1024);
            wArtist = Graphics::Plotter::AddRtoRFunction(Slab::Naked(mFieldsGraph),
                                                         hamiltonian.getGradientDensity(), W_style, "(𝜕ₓϕ)²", 1024);
            uArtist = Graphics::Plotter::AddRtoRFunction(Slab::Naked(mFieldsGraph),
                                                         hamiltonian.getEnergyDensity(), U_style, "E/L", 1024);
        }

        addWindow(Slab::Naked(mFieldsGraph));
        addWindow(Slab::Naked(mEnergyGraph));

        setColumnRelativeWidth(0, 0.4);
        setColumnRelativeWidth(1, -1);
    }

    void RealtimePanel::draw() {
        updateEnergyData();

        fix V_str = hamiltonian.getThePotential()->symbol();

        int errorCount = 0;

        CHECK_GL_ERRORS(errorCount++)

        // imGuiWindow.draw();

        // ************************ RT MONITOR**********************************
        guiWindow.AddExternalDraw([this, V_str]() {
            if (ImGui::CollapsingHeader("Real-time monitor")) {

                if (ImGui::Checkbox(("Show V(ϕ)=" + V_str).c_str(), &showPot)) { vArtist->setVisibility(showPot); }
                if (ImGui::Checkbox("Show (𝜕ₜϕ)²/2", &showKineticEnergy)) { kArtist->setVisibility(showKineticEnergy); }
                if (ImGui::Checkbox("Show (𝜕ₓϕ)²/2", &showGradientEnergy)) {
                    wArtist->setVisibility(showGradientEnergy);
                }
                if (ImGui::Checkbox("Show e", &showEnergyDensity)) { uArtist->setVisibility(showEnergyDensity); }
            }
        });

        CHECK_GL_ERRORS(errorCount++)

        RtoRPanel::draw();
    }

    void RealtimePanel::setSimulationHistory(R2toR::NumericFunction_constptr simHistory,
                                             const Graphics::R2toRFunctionArtist_ptr &simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simHistory, simHistoryGraph);

        auto simulationHistoryGraph = Slab::New<Graphics::Plot2DWindow>("Simulation history", guiWindow.GetGUIContext());
        simulationHistoryGraph->addArtist(simHistoryGraph);

        addWindow(simulationHistoryGraph, ADD_NEW_COLUMN);
        setColumnRelativeWidth(1, -1);
    }

    void RealtimePanel::setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory,
                                               const DFTDataHistory &dftData,
                                               const Graphics::R2toRFunctionArtist_ptr &sftHistoryArtist) {
        RtoRPanel::setSpaceFourierHistory(sftHistory, dftData, sftHistoryArtist);

        auto sftHistoryGraph = Slab::New<Plot2DWindow>("Space Fourier transform", guiWindow.GetGUIContext());
        sftHistoryGraph->addArtist(sftHistoryArtist);

        addWindow(sftHistoryGraph);
    }

    void RealtimePanel::updateEnergyData() {
        const auto t = lastPacket.getSteps()*params->getdt();

        auto U = hamiltonian.getTotalEnergy();
        auto K = hamiltonian.getTotalKineticEnergy();
        auto W = hamiltonian.getTotalGradientEnergy();
        auto V = hamiltonian.getTotalPotentialEnergy();

        auto factor = 1.0 / params->getL();

        UHistoryData.addPoint({t, U * factor});
        KHistoryData.addPoint({t, K * factor});
        WHistoryData.addPoint({t, W * factor});
        VHistoryData.addPoint({t, V * factor});

        auto xMax = t;

        auto yMin = min(UHistoryData.getMin().y,
                        min(KHistoryData.getMin().y, min(WHistoryData.getMin().y, VHistoryData.getMin().y)));
        auto yMax = max(UHistoryData.getMax().y,
                        min(KHistoryData.getMax().y, min(WHistoryData.getMax().y, VHistoryData.getMax().y)));

        mEnergyGraph.getRegion().animate_yMax(yMax);
        mEnergyGraph.getRegion().animate_yMin(yMin);

        mEnergyGraph.getRegion().animate_xMax(xMax);
    }

}