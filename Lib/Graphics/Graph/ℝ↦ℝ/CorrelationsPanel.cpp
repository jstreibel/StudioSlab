//
// Created by joao on 03/05/24.
//

#include "CorrelationsPanel.h"

namespace Graphics {
    CorrelationsPanel::CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian,
                "Correlations",
                "panel for computing and visualizing correlations over simulation history data") {

    }

    void CorrelationsPanel::draw() {
        guiWindow.begin();

        guiWindow.end();

        WindowPanel::draw();
    }

    void CorrelationsPanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                                 std::shared_ptr<HistoryDisplay> simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryGraph);

        addWindow(this->simulationHistoryGraph);
    }


} // Graphics