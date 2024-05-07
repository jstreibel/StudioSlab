//
// Created by joao on 03/05/24.
//

#ifndef STUDIOSLAB_CORRELATIONSPANEL_H
#define STUDIOSLAB_CORRELATIONSPANEL_H

#include "RtoRPanel.h"

namespace Graphics {

    class CorrelationsPanel : public RtoRPanel {
    public:
        void draw() override;

    public:
        CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<HistoryDisplay> simHistoryGraph) override;
    };

} // Graphics

#endif //STUDIOSLAB_CORRELATIONSPANEL_H
