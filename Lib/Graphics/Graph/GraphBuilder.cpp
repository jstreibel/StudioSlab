//
// Created by joao on 21/05/24.
//

#include "GraphBuilder.h"

namespace Graphics {
    PointSetArtist_ptr
    GraphBuilder::AddPointSet(Graph2D_ptr graph, const Math::PointSet_ptr &pointSet, PlotStyle plotStyle, Str label, bool affectsGraphRanges) {
        auto artist = Slab::New<PointSetArtist>(pointSet, plotStyle);
        artist->setLabel(label);

        artist->setAffectGraphRanges(affectsGraphRanges);

        graph->addArtist(artist);

        return artist;
    }

    ParametricCurve2DArtist_ptr
    GraphBuilder::AddCurve(Graph2D_ptr graph, const RtoR2::ParametricCurve_ptr &curve, PlotStyle plotStyle, Str label) {
        auto artist = Slab::New<ParametricCurve2DArtist>(curve, plotStyle);
        artist->setLabel(label);

        graph->addArtist(artist);

        return artist;
    }

    RtoRFunctionArtist_ptr
    GraphBuilder::AddRtoRFunction(Graph2D_ptr graph, const RtoR::Function_ptr &function, PlotStyle plotStyle, Str label, Resolution samples) {
        auto artist = Slab::New<RtoRFunctionArtist>(function, plotStyle, samples);
        artist->setLabel(label);

        graph->addArtist(artist);

        return artist;
    }

    RtoRFunctionArtist_ptr
    GraphBuilder::AddRtoRDiscreteFunction(Graph2D_ptr, const RtoR::DiscreteFunction_ptr &, PlotStyle, Str label) {
        NOT_IMPLEMENTED
    }

    R2toRFunctionArtist_ptr GraphBuilder::AddR2toRFunction(Graph2D_ptr graph, const R2toR::DiscreteFunction_ptr &function, Str label) {
        auto artist = Slab::New<R2toRFunctionArtist>();
        artist->setLabel(label);

        artist->setFunction(function);

        graph->addArtist(artist);

        return artist;
    }

    HistoryArtist_ptr GraphBuilder::AddRtoRHistory(Graph2D_ptr graph, const R2toR::DiscreteFunction_ptr &function, Str label) {
        auto artist = Slab::New<HistoryArtist>();
        artist->setLabel(label);

        artist->setFunction(function);

        graph->addArtist(artist);

        return artist;
    }


} // Graphics