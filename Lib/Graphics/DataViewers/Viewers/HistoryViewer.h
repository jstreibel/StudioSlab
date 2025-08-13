//
// Created by joao on 8/11/24.
//

#ifndef STUDIOSLAB_HISTORYVIEWER_H
#define STUDIOSLAB_HISTORYVIEWER_H

#include "Viewer.h"

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"

namespace Slab::Graphics {

    class HistoryViewer : public Viewer {
        TPointer<FPlot2DWindow> history_window = nullptr;
        TPointer<FPlot2DWindow> xft_history_window = nullptr;

        TPointer<Math::R2toC::NumericFunction> xFourierTransform = nullptr;
        TPointer<Math::R2toR::FNumericFunction> xft_amplitudes = nullptr;

        TPointer<R2toRFunctionArtist> function_artist = nullptr;
        TPointer<R2toRFunctionArtist> xft_amplitudes_artist = nullptr;

        TPointer<R2SectionArtist> section_artist = nullptr;
        TPointer<R2SectionArtist> dft_section_artist = nullptr;

        int oversampling = 12;
        DevFloat curr_t = .0;

        TPointer<RtoR2::StraightLine> function_section = nullptr;
        TPointer<RtoR2::StraightLine> dft_section = nullptr;

        void computeTransform();
    public:
        explicit HistoryViewer(const TPointer<FGUIWindow> &guiWindow, const TPointer<R2toR::FNumericFunction> &function = nullptr);

        Str GetName() const override;

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        void NotifyBecameVisible() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_HISTORYVIEWER_H
