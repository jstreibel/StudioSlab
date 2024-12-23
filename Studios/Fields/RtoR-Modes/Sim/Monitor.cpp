//
// Created by joao on 3/09/23.
//

#include "Monitor.h"

#include <memory>

// Don't touch
#define ODD true
#define DONT_AFFECT_RANGES false

// Options
#define CUSTOM_TICKS false

namespace Modes {
    Monitor::Monitor(const Pointer<KGNumericConfig> &params, Slab::Models::KGRtoR::KGEnergy &hamiltonian,
                     const Str &name)
    : Slab::Models::KGRtoR::Monitor(params, hamiltonian, name)
    {
    }

    void Monitor::setInputModes(RealVector A, const RealVector& k, RealVector Ω) {
        assert(A.size() == Ω.size());

        fix N = A.size();
        auto modes = std::make_shared<Math::PointSet>();
        for (auto i=0; i<N; ++i ) {
            fix ω = Ω[i];
            modes->addPoint({ω, 0.0});
            modes->addPoint({ω, A[i]});
        }

        /*
        mSpaceFourierModesGraph.addPointSet(modes,
            Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].permuteColors(ODD),
            "A(ω)", DONT_AFFECT_RANGES);
            */

        /*
        if(CUSTOM_TICKS) {
            Graphics::AxisArtist::Ticks ticks;
            auto unit = mSpaceFourierModesGraph.getAxisArtist().getHorizontalUnit();
            fix k = Ω[0];
            for (int n = 1; n < 20; ++n) {
                ticks.push_back(Graphics::AxisArtist::Tick{(2 * n - 1) * k, unit((2 * n - 1) * k, 0)});
            }
            this->mFullSpaceFTHistoryDisplay.getAxisArtist().setHorizontalAxisTicks(ticks);
        }*/
    }
} // Modes