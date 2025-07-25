//
// Created by joao on 10/6/24.
//

#ifndef STUDIOSLAB_TWOPOINTCORRELATIONVIEWER_KG_H
#define STUDIOSLAB_TWOPOINTCORRELATIONVIEWER_KG_H

#include "KGViewer.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "Models/KleinGordon/RtoR/Analytic/TwoPointCorrelation.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"

namespace Slab::Models::KGRtoR {

    using PlottingWindow = Graphics::FPlot2DWindow;
    using FunctionArtist = Graphics::R2toRFunctionArtist;
    using SectionArtist = Graphics::R2SectionArtist;
    using SectionLine = Math::RtoR2::StraightLine;
    using Section = Math::RtoR::Section1D;
    using Function = Math::R2toR::FNumericFunction;

    class TwoPointCorrelationViewer_KG : public KGViewer {
        TPointer<FunctionArtist> twoPointArtist = New<FunctionArtist>();
        TPointer<SectionArtist> sectionArtist = New<SectionArtist>();
        TPointer<Graphics::RtoRFunctionArtist> ddt2ptSection;

        TPointer<SectionLine> time_slice = New<SectionLine>();
        TPointer<SectionLine> space_slice = New<SectionLine>();
        TPointer<Section> section_map = New<Section>();

        TPointer<Function> twoPointFunction;
        TwoPointCorrelation twoPointCorrelationAnalytic;
        CorrelationDecay powerDecayCorrelation;


        bool auto_update_Ftx = false;
        float t0{0};
        float Δt{1};
        void computeTwoPointCorrelation();
    public:
        explicit TwoPointCorrelationViewer_KG(const TPointer<Graphics::FGUIWindow> &guiWindow,
                                     const TPointer<R2toR::FNumericFunction> &func = nullptr,
                                     const TPointer<R2toR::FNumericFunction> &ddtFunc = nullptr);

        Str GetName() const override;

        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TWOPOINTCORRELATIONVIEWER_KG_H
