//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

namespace Slab::Models::KGRtoR {

    class CorrelationsPanel : public RtoRPanel {
        Plot2DWindow DFT2DGraph;

        Pointer<R2toRFunctionArtist> ftAmplitudesArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> ftPhasesArtist =     Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> ftRealPartsArtist =  Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> ftImagPartsArtist =  Slab::New<R2toRFunctionArtist>();

        R2toC::NumericFunction_ptr dftFunction;

        Plot2DWindow correlationGraph;
        R2toRFunctionArtist_ptr twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();;

        void computeAll(bool discardRedundantModes);
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    public:
        CorrelationsPanel(const Pointer<KGNumericConfig> &params, GUIWindow &guiWindow, KGEnergy &hamiltonian);

        void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;

        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
