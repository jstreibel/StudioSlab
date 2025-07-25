//
// Created by joao on 8/11/24.
//

#ifndef STUDIOSLAB_MODESHISTORYVIEWER_H
#define STUDIOSLAB_MODESHISTORYVIEWER_H

#include "Viewer.h"

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Graphics/Plot2D/Artists/ParametricCurve2DArtist.h"

namespace Slab::Graphics {

    class ModesHistoryViewer : public Viewer {
        TPointer<Math::R2toC::NumericFunction> xFourierTransform = nullptr;
        TPointer<Math::R2toR::FNumericFunction> xft_amplitudes = nullptr;

        TPointer<R2toRFunctionArtist> xft_amplitudes_artist = nullptr;
        TPointer<R2SectionArtist> modes_artist = nullptr;

        Vector<TPointer<FArtist>> curves_artists;

        TPointer<FPlot2DWindow> xft_history_window;
        TPointer<FPlot2DWindow> modes_window;

        int n_modes = 4;
        float base_mode = M_PI;

        void computeTransform();
        void setupModes();
    public:
        explicit ModesHistoryViewer(const TPointer<FGUIWindow> &);

        Str GetName() const override;

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        void NotifyBecameVisible() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_MODESHISTORYVIEWER_H
