//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

namespace Slab::Models::KGRtoR {

    class CorrelationsPanel : public FRtoRPanel {
        FPlot2DWindow DFT2DGraph;

        TPointer<R2toRFunctionArtist> ftAmplitudesArtist = Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> ftPhasesArtist =     Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> ftRealPartsArtist =  Slab::New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> ftImagPartsArtist =  Slab::New<R2toRFunctionArtist>();

        R2toC::NumericFunction_ptr dftFunction;

        FPlot2DWindow correlationGraph;
        R2toRFunctionArtist_ptr twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();;

        void computeAll(bool discardRedundantModes);
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    public:
        CorrelationsPanel(const TPointer<KGNumericConfig> &params, FGUIWindow &guiWindow, KGEnergy &hamiltonian);

        void SetSimulationHistory(R2toR::FNumericFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;

        void ImmediateDraw(const FPlatformWindow&) override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
