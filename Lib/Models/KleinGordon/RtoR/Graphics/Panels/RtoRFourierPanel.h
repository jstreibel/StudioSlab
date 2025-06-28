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

    class RtoRFourierPanel : public RtoRPanel {
        Pointer<Plot2DWindow> kSpaceGraph;
        Pointer<Plot2DWindow> ωSpaceGraph;
        Pointer<Plot2DWindow> xSpaceGraph;
        Pointer<Plot2DWindow> ωkSpaceGraph;

        Pointer<R2toRFunctionArtist> inv_kSpaceArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> inv_kSpace;

        Pointer<R2toRFunctionArtist> ωSpaceArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> ωSpace;


        Pointer<R2toRFunctionArtist> twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> timeFilteredArtist = Slab::New<R2toRFunctionArtist>();


        Pointer<R2toRFunctionArtist> powerArtist        = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> amplitudesArtist   = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> phasesArtist       = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> realPartsArtist    = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> imagPartsArtist    = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toC::NumericFunction> dft2DFunction;

        static auto FilterSpace(Pointer<const R2toR::NumericFunction> func, DevFloat tMin, DevFloat tMax) -> Pointer<R2toR::NumericFunction>;

        void computeAll(DevFloat t_0, DevFloat t_f);
        void computeFullDFT2D(DevFloat t_0, DevFloat t_f, bool discardRedundantModes);
        void computeTwoPointCorrelations();

        DevFloat kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);
        void computeTimeDFT(DevFloat tMin, DevFloat tMax);

    public:
        RtoRFourierPanel(const Pointer<KGNumericConfig> &params, KGEnergy &hamiltonian, GUIWindow &guiWindow);

        void draw() override;

        void setSimulationHistory(Pointer<const R2toR::NumericFunction> simulationHistory,
                                  const Pointer<R2toRFunctionArtist> &simHistoryGraph) override;

        void setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory, const DFTDataHistory &history,
                                    const R2toRFunctionArtist_ptr &functionArtist) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
