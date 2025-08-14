//
// Created by joao on 6/17/24.
//

#ifndef STUDIOSLAB_RTORHISTORYPANEL_H
#define STUDIOSLAB_RTORHISTORYPANEL_H

#include "Math/Function/RtoR2/StraightLine.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    class FRtoRHistoryPanel : public FRtoRPanel {
        TPointer<Math::RtoR2::StraightLine> xLine;
        TPointer<Math::RtoR2::StraightLine> kLine;

        TPointer<Graphics::FPlot2DWindow> xSection;
        TPointer<Graphics::FPlot2DWindow> xSpaceHistory;
        TPointer<Graphics::FPlot2DWindow> kSection;
        TPointer<Graphics::FPlot2DWindow> kSpaceHistory;

    public:
        FRtoRHistoryPanel(const TPointer<FKGNumericConfig> &params, FGUIWindow &guiWindow, FKGEnergy &hamiltonian);

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetSimulationHistory(TPointer<const R2toR::FNumericFunction>
 simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryGraph) override;

        void SetSpaceFourierHistory(TPointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;

        auto get_kSectionWindow() -> TPointer<Graphics::FPlot2DWindow>;;

    };

} // Slab::Models::RGRtoR

#endif //STUDIOSLAB_RTORHISTORYPANEL_H
