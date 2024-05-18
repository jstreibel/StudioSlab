//
// Created by joao on 20/04/24.
//

#ifndef STUDIOSLAB_RTORFOURIERPANEL_H
#define STUDIOSLAB_RTORFOURIERPANEL_H

#include "RtoRPanel.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Math/Function/RtoR/Calc/DFTInverse.h"

namespace Graphics {

    class RtoRFourierPanel : public RtoRPanel {
        std::shared_ptr<FlatFieldDisplay> inverseFTDisplay;
        std::shared_ptr<R2toR::DiscreteFunction> inverseDFT;

        std::shared_ptr<FlatFieldDisplay> timeFTDisplay;
        std::shared_ptr<R2toR::DiscreteFunction> timeDFT;

        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;

        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);

        void computeTimeDFT(Real tMin, Real tMax);

    public:
        RtoRFourierPanel(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, GUIWindow &guiWindow);

        void draw() override;

        void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                    const DFTDataHistory &dftData,
                                    std::shared_ptr<HistoryDisplay> sftHistoryGraph) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
