//
// Created by joao on 18/04/24.
//

#include "RtoRPanel.h"

namespace Graphics {

    RtoRPanel::RtoRPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian,
                         const Str &name, const Str &description)
            : Socket(params, name, 1, description)
            , guiWindow(guiWindow)
            , params(params)
            , hamiltonian(hamiltonian) {}

    void RtoRPanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simHistory) {
        simulationHistory = simHistory;
    }

    void RtoRPanel::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory) {
        spaceFTHistory = sftHistory;
    }

    void RtoRPanel::handleOutput(const OutputPacket &packet) {

    }


} // Graphics