//
// Created by joao on 3/09/23.
//

#include "Monitor.h"

#define ODD true
#define DONT_AFFECT_RANGES false

namespace Modes {
    Monitor::Monitor(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, Real phiMin, Real phiMax,
                     const Str &name, bool showEnergyHistoryAsDensities)
    : RtoR::Monitor(params, hamiltonian, phiMin, phiMax, name, showEnergyHistoryAsDensities)
    {

    }

    void Monitor::setInputModes(RealVector A, RealVector Ω) {
        assert(A.size() == Ω.size());

        fix N = A.size();
        auto modes = Spaces::PointSet::Ptr(new Spaces::PointSet);
        for (auto i=0; i<N; ++i ) {
            fix ω = Ω[i];
            modes->addPoint({ω, 0.0});
            modes->addPoint({ω, A[i]});
        }

        mSpaceFourierModesGraph.addPointSet(modes, Styles::GetColorScheme()->funcPlotStyles[0].permuteColors(ODD), "A(ω)", DONT_AFFECT_RANGES);
    }
} // Modes