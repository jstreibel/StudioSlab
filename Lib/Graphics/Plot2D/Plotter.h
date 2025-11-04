//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_PLOTTER_H
#define STUDIOSLAB_PLOTTER_H

#include "Math/Function/RtoR2/ParametricCurve.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

#include "Graphics/Plot2D/Artists/PointSetArtist.h"
#include "Graphics/Plot2D/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/HistoryArtist.h"
#include "Graphics/Plot2D/Artists/R2SectionArtist.h"

#include "Plot2DWindow.h"


namespace Slab::Graphics {

    class FPlotter final {
        TPointer<FPlot2DWindow> plottingWindow;

    public:
        explicit FPlotter(TPointer<FPlot2DWindow>);

        // Core::FPayloadData SendRequest(Core::FPayload request) override;

        auto addPointSet             (const Math::FPointSet_ptr&,               PlotStyle,          Str name="", bool affectsGraphRanges=true, int zOrder=0) -> PointSetArtist_ptr;
        auto addCurve                (const Math::RtoR2::ParametricCurve_ptr&, PlotStyle,   const Str& name, int zOrder=0) -> ParametricCurve2DArtist_ptr;
        auto addRtoRFunction         (const Math::RtoR::Function_ptr&,         PlotStyle,          Str name, Resolution samples, int zOrder=0) -> RtoRFunctionArtist_ptr;
        auto addRtoRNumericFunction  (const Math::RtoR::NumericFunction_ptr&,  PlotStyle,          Str name, int zOrder=0) -> RtoRFunctionArtist_ptr;
        auto addR2toRFunction        (const Math::R2toR::FNumericFunction_constptr&,                     Str name, int zOrder=0) -> R2toRFunctionArtist_ptr;
        auto addRtoRHistory          (const Math::R2toR::FNumericFunction_constptr&,                     Str name, int zOrder=0) -> HistoryArtist_ptr;
        auto addR2Section            (const Math::R2toR::Function_constptr&,                       Str name, int zOrder=0) -> R2SectionArtist_ptr;

        static auto AddPointSet     (const TPointer<FPlot2DWindow>&, const Math::FPointSet_ptr&,
                                     PlotStyle, Str name="", bool affectsGraphRanges=true, int zOrder=0)
                                     -> PointSetArtist_ptr;
        static auto AddCurve        (const TPointer<FPlot2DWindow>&, const Math::RtoR2::ParametricCurve_ptr&,
                                     PlotStyle, Str name, int zOrder=0)
                                     -> ParametricCurve2DArtist_ptr;
        static auto AddRtoRFunction (const TPointer<FPlot2DWindow>&, const Math::RtoR::Function_ptr&,
                                     PlotStyle, Str name, Resolution samples=1000, int zOrder=0)
                                     -> RtoRFunctionArtist_ptr;
        static auto AddRtoRNumericFunction (TPointer<FPlot2DWindow>, const Math::RtoR::NumericFunction_ptr&,
                                            PlotStyle,  Str name, int zOrder=0)
                                            -> RtoRFunctionArtist_ptr;
        static auto AddR2toRFunction(const TPointer<FPlot2DWindow>&, const Math::R2toR::FNumericFunction_constptr&,
                                     Str name, int zOrder=0)
                                     -> R2toRFunctionArtist_ptr;
        static auto AddRtoRHistory  (const TPointer<FPlot2DWindow>&, const Math::R2toR::FNumericFunction_constptr&, Str name, int zOrder=0)
                                    -> HistoryArtist_ptr;
        static auto AddR2Section    (const TPointer<FPlot2DWindow>&, const Math::R2toR::Function_constptr&, Str name, int zOrder=0)
                                    -> R2SectionArtist_ptr;
    };

} // Graphics

#endif //STUDIOSLAB_PLOTTER_H
