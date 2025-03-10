//
// Created by joao on 3/09/23.
//

#include "Monitor.h"

#include <memory>

#include "Graphics/Plot2D/PlotThemeManager.h"

// Don't touch
#define ODD true
#define DONT_AFFECT_RANGES false

// Options
#define CUSTOM_TICKS true

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

        // fullSFTHistoryArtist.addPointSet(modes,
        //     Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0].permuteColors(ODD),
        //     "A(ω)", DONT_AFFECT_RANGES);

        if(CUSTOM_TICKS) {
            Graphics::AxisArtist::Ticks ticks;
            auto unit = Constants::π;
            for (int n = 1; n < 20; ++n) {
                ticks.push_back(Graphics::AxisArtist::Tick{(2 * n - 1) * k[0], unit((2 * n - 1) * k[0], 0)});
            }
            this->historyPanel->get_kSectionWindow()->getAxisArtist().setHorizontalAxisTicks(ticks);
        }
    }
} // Modes