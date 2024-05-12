//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "RtoRPanel.h"
#include "Math/Function/Maps/R2toC/R2toCDiscreteFunction.h"
#include "Math/Function/Maps/R2toR/Model/FunctionsCollection/ComplexMagnitudeFunction.h"

namespace Graphics {

    class CorrelationsPanel : public RtoRPanel {
        FlatFieldDisplay DFT2DGraph;
        R2toR::ComplexMagnitudeFunction ftAmplitudes;

        void computeFullDFT2D();

    public:
        CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<HistoryDisplay> simHistoryGraph) override;

        void draw() override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
