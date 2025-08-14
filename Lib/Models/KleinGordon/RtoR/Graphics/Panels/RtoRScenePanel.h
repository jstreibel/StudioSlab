//
// Created by joao on 24/05/24.
//

#ifndef STUDIOSLAB_RTORSCENEPANEL_H
#define STUDIOSLAB_RTORSCENEPANEL_H

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Graphics/Plot3D/Scene3DWindow.h"

namespace Slab::Models::KGRtoR {

    class RtoRScenePanel : public FRtoRPanel {
        Graphics::Scene3DWindow_ptr scene;

    public:
        RtoRScenePanel(const TPointer<FKGNumericConfig> &params,
                       FGUIWindow &guiWindow,
                       FKGEnergy &hamiltonian);

        void SetSimulationHistory(TPointer<const R2toR::FNumericFunction>
 simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;

        void SetSpaceFourierHistory(TPointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;
    };

    DefinePointers(RtoRScenePanel)

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_RTORSCENEPANEL_H
