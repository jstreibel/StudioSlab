//
// Created by joao on 21/05/24.
//

#ifndef STUDIOSLAB_GRAPHBUILDER_H
#define STUDIOSLAB_GRAPHBUILDER_H

#include "Math/Function/RtoR2/ParametricCurve.h"
#include "Math/Function/RtoR/Model/RtoRDiscreteFunction.h"

#include "Graphics/Graph/Artists/PointSetArtist.h"
#include "Graphics/Graph/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Graph/Artists/RtoRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/HistoryArtist.h"

#include "Graph.h"

namespace Graphics {

    class GraphBuilder {
    public:

        static PointSetArtist_ptr          AddPointSet     (Graph2D_ptr, const ::Math::PointSet_ptr&,          PlotStyle, Str name="", bool affectsGraphRanges=true);
        static ParametricCurve2DArtist_ptr AddCurve        (Graph2D_ptr, const ::RtoR2::ParametricCurve_ptr&,  PlotStyle, Str name);
        static RtoRFunctionArtist_ptr      AddRtoRFunction (Graph2D_ptr, const ::RtoR::Function_ptr&,          PlotStyle, Str name, Resolution samples);
        static RtoRFunctionArtist_ptr      AddRtoRDiscreteFunction (Graph2D_ptr, const ::RtoR::DiscreteFunction_ptr&,  PlotStyle, Str name);
        static R2toRFunctionArtist_ptr     AddR2toRFunction(Graph2D_ptr, const ::R2toR::DiscreteFunction_ptr&,            Str name);
        static HistoryArtist_ptr           AddRtoRHistory  (Graph2D_ptr, const ::R2toR::DiscreteFunction_ptr&,            Str name);
    };

} // Graphics

#endif //STUDIOSLAB_GRAPHBUILDER_H
