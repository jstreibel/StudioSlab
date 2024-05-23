//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_PLOTTER_H
#define STUDIOSLAB_PLOTTER_H

#include "Math/Function/RtoR2/ParametricCurve.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"

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

        PointSetArtist_ptr          addPointSet             (const Math::PointSet_ptr&,          PlotStyle,          Str name="", bool affectsGraphRanges=true);
        ParametricCurve2DArtist_ptr addCurve                (const Math::RtoR2::ParametricCurve_ptr&,  PlotStyle,          const Str& name);
        RtoRFunctionArtist_ptr      addRtoRFunction         (const Math::RtoR::Function_ptr&,          PlotStyle,          Str name, Resolution samples);
        RtoRFunctionArtist_ptr      addRtoRDiscreteFunction (const Math::RtoR::DiscreteFunction_ptr&,  PlotStyle,          Str name);
        R2toRFunctionArtist_ptr     addR2toRFunction        (const Math::R2toR::DiscreteFunction_ptr&,                     Str name);
        HistoryArtist_ptr           addRtoRHistory          (const Math::R2toR::DiscreteFunction_ptr&,                     Str name);
        R2SectionArtist_ptr         addR2Section            (const Math::R2toR::Function_ptr&,                             Str name);

        static PointSetArtist_ptr          AddPointSet     (PlottingWindow_ptr, const Math::PointSet_ptr&,          PlotStyle,          Str name="", bool affectsGraphRanges=true);
        static ParametricCurve2DArtist_ptr AddCurve        (PlottingWindow_ptr, const Math::RtoR2::ParametricCurve_ptr&,  PlotStyle,          Str name);
        static RtoRFunctionArtist_ptr      AddRtoRFunction (PlottingWindow_ptr, const Math::RtoR::Function_ptr&,          PlotStyle,          Str name, Resolution samples);
        static RtoRFunctionArtist_ptr      AddRtoRDiscreteFunction (PlottingWindow_ptr, const Math::RtoR::DiscreteFunction_ptr&,  PlotStyle,  Str name);
        static R2toRFunctionArtist_ptr     AddR2toRFunction(PlottingWindow_ptr, const Math::R2toR::DiscreteFunction_ptr&,                     Str name);
        static HistoryArtist_ptr           AddRtoRHistory  (PlottingWindow_ptr, const Math::R2toR::DiscreteFunction_ptr&,                     Str name);
        static R2SectionArtist_ptr         AddR2Section    (PlottingWindow_ptr, const Math::R2toR::Function_ptr&,                             Str name);
    };

} // Graphics

#endif //STUDIOSLAB_PLOTTER_H
