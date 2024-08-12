//
// Created by joao on 8/11/24.
//

#ifndef STUDIOSLAB_MODESHISTORYVIEWER_H
#define STUDIOSLAB_MODESHISTORYVIEWER_H

#include "Viewer.h"

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2SectionArtist.h"

namespace Slab::Graphics {

    class ModesHistoryViewer : public Viewer {
        Pointer<Math::R2toC::NumericFunction> xFourierTransform = nullptr;
        Pointer<Math::R2toR::NumericFunction> xft_amplitudes = nullptr;

        Pointer<R2toRFunctionArtist> xft_amplitudes_artist = nullptr;

        Pointer<PlottingWindow> xft_history_window;
        Pointer<PlottingWindow> modes_window;

        float base_mode = M_PI;

        void computeTransform();
    public:
        explicit ModesHistoryViewer(const Pointer<GUIWindow> &);

        void draw() override;

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void notifyBecameVisible() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_MODESHISTORYVIEWER_H
