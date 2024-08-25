//
// Created by joao on 8/11/24.
//

#ifndef STUDIOSLAB_HISTORYVIEWER_H
#define STUDIOSLAB_HISTORYVIEWER_H

#include "Viewer.h"

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2SectionArtist.h"

namespace Slab::Graphics {

    class HistoryViewer : public Viewer {
        Pointer<PlottingWindow> history_window = nullptr;
        Pointer<PlottingWindow> xft_history_window = nullptr;

        Pointer<Math::R2toC::NumericFunction> xFourierTransform = nullptr;
        Pointer<Math::R2toR::NumericFunction> xft_amplitudes = nullptr;

        Pointer<R2toRFunctionArtist> function_artist = nullptr;
        Pointer<R2toRFunctionArtist> xft_amplitudes_artist = nullptr;

        Pointer<R2SectionArtist> section_artist = nullptr;
        Pointer<R2SectionArtist> dft_section_artist = nullptr;

        int oversampling = 12;
        Real curr_t = .0;

        Pointer<RtoR2::StraightLine> function_section = nullptr;
        Pointer<RtoR2::StraightLine> dft_section = nullptr;

        void computeTransform();
    public:
        explicit HistoryViewer(const Pointer<GUIWindow> &guiWindow, const Pointer<R2toR::NumericFunction> &function = nullptr);

        void draw() override;

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void notifyBecameVisible() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_HISTORYVIEWER_H
