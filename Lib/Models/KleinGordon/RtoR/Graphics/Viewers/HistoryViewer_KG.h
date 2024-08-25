//
// Created by joao on 25/08/24.
//

#ifndef STUDIOSLAB_HISTORYVIEWER_KG_H
#define STUDIOSLAB_HISTORYVIEWER_KG_H

#include "KGViewer.h"

#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2SectionArtist.h"

namespace Slab::Models::KGRtoR {

    class HistoryViewer : public KGViewer {
        Pointer<Graphics::PlottingWindow> history_window = nullptr;
        Pointer<Graphics::PlottingWindow> slice_window = nullptr;

        Pointer<Graphics::R2toRFunctionArtist> function_artist = nullptr;
        Pointer<Graphics::R2toRFunctionArtist> ddt_function_artist = nullptr;
        Pointer<Graphics::R2SectionArtist> function_section_artist = nullptr;
        Pointer<Graphics::R2SectionArtist> ddt_function_section_artist = nullptr;

        Pointer<RtoR2::StraightLine> function_section = nullptr;

        int oversampling = 12;
        Real dt = -1;
        int curr_ti = 0;

    public:
        explicit HistoryViewer(const Pointer<Graphics::GUIWindow> &guiWindow);

        void draw() override;

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void setFunctionDerivative(FuncPointer pointer) override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_HISTORYVIEWER_KG_H
