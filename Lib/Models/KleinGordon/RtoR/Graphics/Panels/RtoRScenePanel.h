//
// Created by joao on 24/05/24.
//

#ifndef STUDIOSLAB_RTORSCENEPANEL_H
#define STUDIOSLAB_RTORSCENEPANEL_H

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Graphics/Graph3D/Scene3DWindow.h"

namespace Slab::Models::KGRtoR {

    class RtoRScenePanel : public RtoRPanel {
        Graphics::Scene3DWindow_ptr scene;

    public:
        RtoRScenePanel(const NumericConfig &params,
                       GUIWindow &guiWindow,
                       KGEnergy &hamiltonian);

        void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;

        void setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory, const DFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;
    };

    DefinePointers(RtoRScenePanel)

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_RTORSCENEPANEL_H
