//
// Created by joao on 18/04/24.
//

#include "RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    RtoRPanel::RtoRPanel(Pointer<KGNumericConfig>params, GUIWindow &guiWindow, KGEnergy &hamiltonian,
                         const Str &name, const Str &description)
            : Socket(name, 1, description)
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
                                           const DFTDataHistory &dftDataHistory,
                                           const Graphics::R2toRFunctionArtist_ptr& sftHistoryArtist) {

        spaceFTHistory = sftHistory;
        this->dftData = &dftDataHistory;
        spaceFTHistoryArtist = sftHistoryArtist;
    }

    void RtoRPanel::handleOutput(const Math::OutputPacket &packet) {

    }

    void RtoRPanel::notifyBecameVisible() {

    }

    void RtoRPanel::notifyBecameInvisible() {

    }


} // Graphics