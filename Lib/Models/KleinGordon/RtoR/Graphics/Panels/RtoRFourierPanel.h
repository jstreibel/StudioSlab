//
// Created by joao on 20/04/24.
//

#ifndef STUDIOSLAB_RTORFOURIERPANEL_H
#define STUDIOSLAB_RTORFOURIERPANEL_H

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"

#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Operations/DFTInverse.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"

#include "Graphics/Plot2D/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

namespace Slab::Models::KGRtoR {

    class RtoRFourierPanel : public FRtoRPanel {
        TPointer<FPlot2DWindow> kSpaceGraph;
        TPointer<FPlot2DWindow> ωSpaceGraph;
        TPointer<FPlot2DWindow> xSpaceGraph;
        TPointer<FPlot2DWindow> ωkSpaceGraph;

        TPointer<R2toRFunctionArtist> inv_kSpaceArtist = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toR::FNumericFunction> inv_kSpace;

        TPointer<R2toRFunctionArtist> ωSpaceArtist = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toR::FNumericFunction> ωSpace;


        TPointer<R2toRFunctionArtist> twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> timeFilteredArtist = Slab::New<R2toRFunctionArtist>();


        TPointer<R2toRFunctionArtist> powerArtist        = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> amplitudesArtist   = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> phasesArtist       = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> realPartsArtist    = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> imagPartsArtist    = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toC::NumericFunction> dft2DFunction;

        static auto FilterSpace(TPointer<const R2toR::FNumericFunction> func, DevFloat tMin, DevFloat tMax) -> TPointer<R2toR::FNumericFunction>;

        void computeAll(DevFloat t_0, DevFloat t_f);
        void computeFullDFT2D(DevFloat t_0, DevFloat t_f, bool discardRedundantModes);
        void computeTwoPointCorrelations();

        DevFloat kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);
        void computeTimeDFT(DevFloat tMin, DevFloat tMax);

    public:
        RtoRFourierPanel(const TPointer<KGNumericConfig> &params, KGEnergy &hamiltonian, FGUIWindow &guiWindow);

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetSimulationHistory(TPointer<const R2toR::FNumericFunction> simulationHistory,
                                  const TPointer<R2toRFunctionArtist> &simHistoryGraph) override;

        void SetSpaceFourierHistory(TPointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &functionArtist) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
