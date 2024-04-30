//
// Created by joao on 20/04/24.
//

#ifndef STUDIOSLAB_RTORFOURIERPANEL_H
#define STUDIOSLAB_RTORFOURIERPANEL_H

#include "RtoRPanel.h"
#include "Maps/RtoR2/StraightLine.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Maps/RtoR/Calc/DFTInverse.h"

namespace Graphics {

    class RtoRFourierPanel : public RtoRPanel {
        std::shared_ptr<FlatFieldDisplay> inverseFTDisplay;
        std::shared_ptr<R2toR::DiscreteFunction> inverseDFT;

        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;

        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);

    public:
        RtoRFourierPanel(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, GUIWindow &guiWindow);

        void draw() override;

        void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                    const SimHistory_DFT::DFTDataHistory &dftData,
                                    std::shared_ptr<HistoryDisplay> sftHistoryGraph) override;

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<HistoryDisplay> simHistoryGraph) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
