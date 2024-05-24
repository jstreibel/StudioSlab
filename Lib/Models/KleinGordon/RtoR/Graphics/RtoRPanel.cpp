//
// Created by joao on 18/04/24.
//

#include "RtoRPanel.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"

namespace Slab::Models::KGRtoR {



    RtoRPanel::RtoRPanel(const Math::NumericConfig &params, GUIWindow &guiWindow, KGEnergy &hamiltonian,
                         const Str &name, const Str &description)
            : Socket(params, name, 1, description)
            , guiWindow(guiWindow)
            , params(params)
            , hamiltonian(hamiltonian)
    {

    }

    void RtoRPanel::setSimulationHistory(Math::R2toR::DiscreteFunction_constptr simHistory,
                                         PlottingWindow_ptr simHistoryGraph) {
        simulationHistory = simHistory;
        simulationHistoryGraph = simHistoryGraph;
    }

    void RtoRPanel::setSpaceFourierHistory(Math::R2toR::DiscreteFunction_constptr sftHistory,
                                           const DFTDataHistory &dftData,
                                           PlottingWindow_ptr sftHistoryGraph) {

        spaceFTHistory = sftHistory;
        this->dftData = &dftData;
        spaceFTHistoryGraph = sftHistoryGraph;
    }

    void RtoRPanel::handleOutput(const Math::OutputPacket &packet) {

    }

    void RtoRPanel::notifyBecameVisible() {

    }

    void RtoRPanel::notifyBecameInvisible() {

    }


} // Graphics