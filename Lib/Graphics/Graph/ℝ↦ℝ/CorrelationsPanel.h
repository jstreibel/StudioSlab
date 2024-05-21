//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "RtoRPanel.h"
#include "Math/Function/R2toC/R2toCDiscreteFunction.h"

namespace Graphics {

    class CorrelationsPanel : public RtoRPanel {
        FlatFieldDisplay DFT2DGraph;
        R2toC::DiscreteFunction_ptr dftFunction;
        R2toR::DiscreteFunction_ptr ftAmplitudes;
        R2toR::DiscreteFunction_ptr ftPhases;
        R2toR::DiscreteFunction_ptr ftRealParts;
        R2toR::DiscreteFunction_ptr ftImagParts;

        FlatFieldDisplay correlationGraph;
        R2toR::DiscreteFunction_ptr powerSpectrum;
        R2toR::DiscreteFunction_ptr twoPointCorrFunction;

        void computeAll(bool discardRedundantModes);
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    public:
        CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian);

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  HistoryDisplay_ptr simHistoryGraph) override;

        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
