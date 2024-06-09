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

    void RtoRPanel::setSimulationHistory(Math::R2toR::NumericFunction_constptr simHistory,
                                         const Graphics::R2toRFunctionArtist_ptr& simHistoryArtist) {
        simulationHistory = simHistory;
        simulationHistoryArtist = simHistoryArtist;
    }

    void RtoRPanel::setSpaceFourierHistory(Math::R2toR::NumericFunction_constptr sftHistory,
                                           const DFTDataHistory &dftData,
                                           const Graphics::R2toRFunctionArtist_ptr& sftHistoryArtist) {

        spaceFTHistory = sftHistory;
        this->dftData = &dftData;
        spaceFTHistoryArtist = sftHistoryArtist;
    }

    void RtoRPanel::handleOutput(const Math::OutputPacket &packet) {

    }

    void RtoRPanel::notifyBecameVisible() {

    }

    void RtoRPanel::notifyBecameInvisible() {

    }


} // Graphics