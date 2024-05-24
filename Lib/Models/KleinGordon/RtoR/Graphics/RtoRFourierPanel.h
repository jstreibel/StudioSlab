//
// Created by joao on 20/04/24.
//

#ifndef STUDIOSLAB_RTORFOURIERPANEL_H
#define STUDIOSLAB_RTORFOURIERPANEL_H

#include "RtoRPanel.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Math/Function/RtoR/Calc/DFTInverse.h"
#include "Graphics/Graph/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"

namespace Slab::Models::KGRtoR {

    class RtoRFourierPanel : public RtoRPanel {
        PlottingWindow_ptr inverseDFTDisplay = Slab::New<PlottingWindow>("ℱₖ⁻¹[ℱ]");
        R2toRFunctionArtist_ptr inverseDFTArtist = Slab::New<R2toRFunctionArtist>();
        R2toR::DiscreteFunction_ptr inverseDFT;

        PlottingWindow_ptr timeDFTDisplay = Slab::New<PlottingWindow>("ℱₜ[ϕ](ω,x)");
        R2toRFunctionArtist_ptr timeDFTArtist = Slab::New<R2toRFunctionArtist>();;
        R2toR::DiscreteFunction_ptr timeDFT;

        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
        ParametricCurve2DArtist_ptr cutoffLineArtist;

        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);

        void computeTimeDFT(Real tMin, Real tMax);

    public:
        RtoRFourierPanel(const NumericConfig &params, KGEnergy &hamiltonian, GUIWindow &guiWindow);

        void draw() override;

        void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                    const DFTDataHistory &dftData,
                                    PlottingWindow_ptr sftHistoryGraph) override;
    };

} // Graphics

#endif //STUDIOSLAB_RTORFOURIERPANEL_H
