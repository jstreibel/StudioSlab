//
// Created by joao on 18/04/24.
//

#include "RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    FRtoRPanel::FRtoRPanel(
        TPointer<FKGNumericConfig>params,
        FGUIWindow &guiWindow,
        FKGEnergy &hamiltonian,
        const Str &name,
        const Str &description)
    : FOutputChannel(name, 1, description)
    , WindowPanel(FSlabWindowConfig(name))
    , guiWindow(guiWindow)
    , Params(params)
    , Hamiltonian(hamiltonian)
    {

    }

    void FRtoRPanel::SetSimulationHistory(Math::R2toR::FNumericFunction_constptr simHistory,
                                         const Graphics::R2toRFunctionArtist_ptr& simHistoryArtist) {
        simulationHistory = simHistory;
        simulationHistoryArtist = simHistoryArtist;
    }

    void FRtoRPanel::SetSpaceFourierHistory(Math::R2toR::FNumericFunction_constptr sftHistory,
                                           const FDFTDataHistory &dftDataHistory,
                                           const Graphics::R2toRFunctionArtist_ptr& sftHistoryArtist) {

        spaceFTHistory = sftHistory;
        this->dftData = &dftDataHistory;
        spaceFTHistoryArtist = sftHistoryArtist;
    }

    void FRtoRPanel::HandleOutput(const Math::OutputPacket &packet) {

    }

    void FRtoRPanel::notifyBecameVisible() {

    }

    void FRtoRPanel::notifyBecameInvisible() {

    }


} // Graphics