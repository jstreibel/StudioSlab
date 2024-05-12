//
// Created by joao on 03/05/24.
//

#include "CorrelationsPanel.h"

#include "imgui.h"
#include "Math/Function/Maps/R2toR/Calc/R2toRDFT.h"

namespace Graphics {
    CorrelationsPanel::CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian,
                "Correlations",
                "panel for computing and visualizing correlations over simulation history data")
    , DFT2DGraph("Spacetime Fourier transform")
    {

    }

    void CorrelationsPanel::draw() {
        guiWindow.begin();

        if(ImGui::CollapsingHeader("Full spacetime FT")){
            if(ImGui::Button("Generate"))
                computeFullDFT2D();
        }

        guiWindow.end();

        WindowPanel::draw();
    }

    void CorrelationsPanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                                 std::shared_ptr<HistoryDisplay> simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryGraph);

        addWindow(this->simulationHistoryGraph);
    }

    void CorrelationsPanel::computeFullDFT2D() {
        auto result = R2toR::R2toRDFT::DFT(*simulationHistory);

        DFT2DGraph.removeFunction(DummyPtr(ftAmplitudes));

        ftAmplitudes.setFunc(result);

        DFT2DGraph.addFunction(DummyPtr(ftAmplitudes), "ℱₜₓ[ϕ](ω,k)");
    }


} // Graphics