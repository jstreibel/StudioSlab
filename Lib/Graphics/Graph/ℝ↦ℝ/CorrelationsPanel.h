//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "RtoRPanel.h"
#include "Math/Function/R2toC/R2toCDiscreteFunction.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"

namespace Graphics {

    class CorrelationsPanel : public RtoRPanel {
        PlottingWindow DFT2DGraph;

        R2toRFunctionArtist_ptr ftAmplitudesArtist = Slab::New<R2toRFunctionArtist>();
        R2toRFunctionArtist_ptr ftPhasesArtist =     Slab::New<R2toRFunctionArtist>();
        R2toRFunctionArtist_ptr ftRealPartsArtist =  Slab::New<R2toRFunctionArtist>();
        R2toRFunctionArtist_ptr ftImagPartsArtist =  Slab::New<R2toRFunctionArtist>();

        R2toC::DiscreteFunction_ptr dftFunction;

        PlottingWindow correlationGraph;
        R2toRFunctionArtist_ptr twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();;

        void computeAll(bool discardRedundantModes);
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    public:
        CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian);

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  PlottingWindow_ptr simHistoryGraph) override;

        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
