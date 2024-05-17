//
// Created by joao on 18/04/24.
//

#include "RtoRPanel.h"
#include "HistoryDisplay.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

namespace Graphics {

    RtoRPanel::RtoRPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian,
                         const Str &name, const Str &description)
            : Socket(params, name, 1, description)
            , guiWindow(guiWindow)
            , params(params)
            , hamiltonian(hamiltonian)
    {

    }

    void RtoRPanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory,
                                         std::shared_ptr<HistoryDisplay> simHistoryGraph) {
        simulationHistory = simHistory;
        simulationHistoryGraph = simHistoryGraph;
    }

    void RtoRPanel::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                           const DFTDataHistory &dftData,
                                           std::shared_ptr<HistoryDisplay> sftHistoryGraph) {

        spaceFTHistory = sftHistory;
        this->dftData = &dftData;
        spaceFTHistoryGraph = sftHistoryGraph;
    }

    void RtoRPanel::handleOutput(const OutputPacket &packet) {

    }

    void RtoRPanel::notifyBecameVisible() {

    }

    void RtoRPanel::notifyBecameInvisible() {

    }


} // Graphics