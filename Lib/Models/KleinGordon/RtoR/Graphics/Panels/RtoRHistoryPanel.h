//
// Created by joao on 6/17/24.
//

#ifndef STUDIOSLAB_RTORHISTORYPANEL_H
#define STUDIOSLAB_RTORHISTORYPANEL_H

#include "Math/Function/RtoR2/StraightLine.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    class RtoRHistoryPanel : public FRtoRPanel {
        Pointer<Math::RtoR2::StraightLine> xLine;
        Pointer<Math::RtoR2::StraightLine> kLine;

        Pointer<Graphics::FPlot2DWindow> xSection;
        Pointer<Graphics::FPlot2DWindow> xSpaceHistory;
        Pointer<Graphics::FPlot2DWindow> kSection;
        Pointer<Graphics::FPlot2DWindow> kSpaceHistory;

    public:
        RtoRHistoryPanel(const Pointer<KGNumericConfig> &params, FGUIWindow &guiWindow, KGEnergy &hamiltonian);

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetSimulationHistory(Pointer<const R2toR::FNumericFunction>
 simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryGraph) override;

        void SetSpaceFourierHistory(Pointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;

        auto get_kSectionWindow() -> Pointer<Graphics::FPlot2DWindow>;;

    };

} // Slab::Models::RGRtoR

#endif //STUDIOSLAB_RTORHISTORYPANEL_H
