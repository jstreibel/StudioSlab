//
// Created by joao on 25/08/24.
//

#ifndef STUDIOSLAB_HISTORYVIEWER_KG_H
#define STUDIOSLAB_HISTORYVIEWER_KG_H

#include "KGViewer.h"

#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"

namespace Slab::Models::KGRtoR {

    class HistoryViewer : public KGViewer {
        TPointer<Graphics::FPlot2DWindow> history_window = nullptr;
        TPointer<Graphics::FPlot2DWindow> slice_window = nullptr;

        TPointer<Graphics::R2toRFunctionArtist> function_artist = nullptr;
        TPointer<Graphics::R2toRFunctionArtist> ddt_function_artist = nullptr;
        TPointer<Graphics::R2toRFunctionArtist> d2dt2_function_artist = nullptr;

        TPointer<Graphics::R2SectionArtist> function_section_artist = nullptr;
        TPointer<Graphics::R2SectionArtist> ddt_function_section_artist = nullptr;
        TPointer<Graphics::R2SectionArtist> d2dt2_function_section_artist = nullptr;

        TPointer<RtoR2::StraightLine> function_section = nullptr;

        TPointer<R2toR::FNumericFunction> d2dt2_function = nullptr;

        int oversampling = 12;
        DevFloat dt = -1;
        int curr_ti = 0;

    public:
        explicit HistoryViewer(const TPointer<Graphics::FGUIWindow> &guiWindow);

        Str GetName() const override;

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        void SetFunctionDerivative(FuncPointer pointer) override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_HISTORYVIEWER_KG_H
