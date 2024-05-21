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
        FlatFieldDisplay_ptr inverseFTDisplay;
        R2toR::DiscreteFunction_ptr inverseDFT;

        FlatFieldDisplay_ptr timeFTDisplay;
        R2toR::DiscreteFunction_ptr timeDFT;

        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;

        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);

        void computeTimeDFT(Real tMin, Real tMax);

    public:
        RtoRFourierPanel(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, GUIWindow &guiWindow);

        void draw() override;

        void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                    const DFTDataHistory &dftData,
                                    HistoryDisplay_ptr sftHistoryGraph) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
