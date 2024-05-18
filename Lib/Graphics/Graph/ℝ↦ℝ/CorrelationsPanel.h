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
        std::shared_ptr<R2toC::DiscreteFunction> dftFunction;
        std::shared_ptr<R2toR::DiscreteFunction> ftAmplitudes;
        std::shared_ptr<R2toR::DiscreteFunction> ftPhases;
        std::shared_ptr<R2toR::DiscreteFunction> ftRealParts;
        std::shared_ptr<R2toR::DiscreteFunction> ftImagParts;

        FlatFieldDisplay correlationGraph;
        std::shared_ptr<R2toR::DiscreteFunction> powerSpectrum;
        std::shared_ptr<R2toR::DiscreteFunction> twoPointCorrFunction;

        void computeAll(bool discardRedundantModes);
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    public:
        CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<HistoryDisplay> simHistoryGraph) override;

        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
