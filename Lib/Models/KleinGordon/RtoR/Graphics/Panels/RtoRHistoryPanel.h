//
// Created by joao on 6/17/24.
//

#ifndef STUDIOSLAB_RTORHISTORYPANEL_H
#define STUDIOSLAB_RTORHISTORYPANEL_H

#include "Math/Function/RtoR2/StraightLine.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    class RtoRHistoryPanel : public RtoRPanel {
        Pointer<Math::RtoR2::StraightLine> xLine;
        Pointer<Math::RtoR2::StraightLine> kLine;

        Pointer<Graphics::Plot2DWindow> xSection;
        Pointer<Graphics::Plot2DWindow> xSpaceHistory;
        Pointer<Graphics::Plot2DWindow> kSection;
        Pointer<Graphics::Plot2DWindow> kSpaceHistory;

    public:
        RtoRHistoryPanel(const NumericConfig &params, GUIWindow &guiWindow, KGEnergy &hamiltonian);

        void draw() override;

        void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryGraph) override;

        void setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory, const DFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;
    };

} // Slab::Models::RGRtoR

#endif //STUDIOSLAB_RTORHISTORYPANEL_H
