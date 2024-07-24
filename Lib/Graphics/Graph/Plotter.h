//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_PLOTTER_H
#define STUDIOSLAB_PLOTTER_H

#include "Math/Function/RtoR2/ParametricCurve.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

#include "Graphics/Graph/Artists/PointSetArtist.h"
#include "Graphics/Graph/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Graph/Artists/RtoRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/HistoryArtist.h"
#include "Graphics/Graph/Artists/R2SectionArtist.h"

#include "PlottingWindow.h"


namespace Slab::Graphics {

    class Plotter {
        PlottingWindow_ptr plottingWindow;
    public:
        explicit Plotter(PlottingWindow_ptr);

        auto addPointSet             (const Math::PointSet_ptr&,               PlotStyle,          Str name="", bool affectsGraphRanges=true, int zOrder=0) -> PointSetArtist_ptr;
        auto addCurve                (const Math::RtoR2::ParametricCurve_ptr&, PlotStyle,   const Str& name, int zOrder=0) -> ParametricCurve2DArtist_ptr;
        auto addRtoRFunction         (const Math::RtoR::Function_ptr&,         PlotStyle,          Str name, Resolution samples, int zOrder=0) -> RtoRFunctionArtist_ptr;
        auto addRtoRNumericFunction  (const Math::RtoR::NumericFunction_ptr&,  PlotStyle,          Str name, int zOrder=0) -> RtoRFunctionArtist_ptr;
        auto addR2toRFunction        (const Math::R2toR::NumericFunction_constptr&,                     Str name, int zOrder=0) -> R2toRFunctionArtist_ptr;
        auto addRtoRHistory          (const Math::R2toR::NumericFunction_constptr&,                     Str name, int zOrder=0) -> HistoryArtist_ptr;
        auto addR2Section            (const Math::R2toR::Function_constptr&,                       Str name, int zOrder=0) -> R2SectionArtist_ptr;

        static auto AddPointSet     (const PlottingWindow_ptr&, const Math::PointSet_ptr&,
                                     PlotStyle, Str name="", bool affectsGraphRanges=true, int zOrder=0)
                                     -> PointSetArtist_ptr;
        static auto AddCurve        (const PlottingWindow_ptr&, const Math::RtoR2::ParametricCurve_ptr&,
                                     PlotStyle, Str name, int zOrder=0)
                                     -> ParametricCurve2DArtist_ptr;
        static auto AddRtoRFunction (const PlottingWindow_ptr&, const Math::RtoR::Function_ptr&,
                                     PlotStyle, Str name, Resolution samples=1000, int zOrder=0)
                                     -> RtoRFunctionArtist_ptr;
        static auto AddRtoRNumericFunction (PlottingWindow_ptr, const Math::RtoR::NumericFunction_ptr&,
                                            PlotStyle,  Str name, int zOrder=0)
                                            -> RtoRFunctionArtist_ptr;
        static auto AddR2toRFunction(const PlottingWindow_ptr&, const Math::R2toR::NumericFunction_constptr&,
                                     Str name, int zOrder=0)
                                     -> R2toRFunctionArtist_ptr;
        static auto AddRtoRHistory  (const PlottingWindow_ptr&, const Math::R2toR::NumericFunction_constptr&, Str name, int zOrder=0)
                                    -> HistoryArtist_ptr;
        static auto AddR2Section    (const PlottingWindow_ptr&, const Math::R2toR::Function_constptr&, Str name, int zOrder=0)
                                    -> R2SectionArtist_ptr;
    };

} // Graphics

#endif //STUDIOSLAB_PLOTTER_H
