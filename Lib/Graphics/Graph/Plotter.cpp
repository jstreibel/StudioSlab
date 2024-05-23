//
// Created by joao on 21/05/24.
//

#include "Plotter.h"

namespace Slab::Graphics {

    Plotter::Plotter(Graphics::PlottingWindow_ptr win) : plottingWindow(win) {}

    PointSetArtist_ptr Plotter::addPointSet(const Math::PointSet_ptr &data, PlotStyle style, Str name, bool affectsGraphRanges) {
        return Plotter::AddPointSet(plottingWindow, data, style, std::move(name), affectsGraphRanges);
    }

    ParametricCurve2DArtist_ptr Plotter::addCurve(const RtoR2::ParametricCurve_ptr &data, PlotStyle style, const Str& name) {
        return Plotter::AddCurve(plottingWindow, data, style, name);
    }

    RtoRFunctionArtist_ptr
    Plotter::addRtoRFunction(const RtoR::Function_ptr &data, PlotStyle style, Str name, Resolution samples) {
        return Plotter::AddRtoRFunction(plottingWindow, data, style, name, samples);
    }

    RtoRFunctionArtist_ptr Plotter::addRtoRDiscreteFunction(const RtoR::DiscreteFunction_ptr &data, PlotStyle style, Str name) {
        return Plotter::AddRtoRDiscreteFunction(plottingWindow, data, style, name);
    }

    R2toRFunctionArtist_ptr Plotter::addR2toRFunction(const R2toR::DiscreteFunction_ptr &data, Str name) {
        return Plotter::AddR2toRFunction(plottingWindow, data, name);
    }

    HistoryArtist_ptr Plotter::addRtoRHistory(const R2toR::DiscreteFunction_ptr &data, Str name) {
        return Plotter::AddRtoRHistory(plottingWindow, data, name);
    }

    R2SectionArtist_ptr Plotter::addR2Section(const R2toR::Function_ptr &data, Str name) {
        return AddR2Section(plottingWindow, data, name);
    }

    PointSetArtist_ptr
    Plotter::AddPointSet(PlottingWindow_ptr graph, const Math::PointSet_ptr &pointSet, PlotStyle plotStyle, Str label, bool affectsGraphRanges) {
        auto artist = Slab::New<PointSetArtist>(pointSet, plotStyle);
        artist->setLabel(label);

        artist->setAffectGraphRanges(affectsGraphRanges);

        graph->addArtist(artist);

        return artist;
    }

    ParametricCurve2DArtist_ptr
    Plotter::AddCurve(PlottingWindow_ptr graph, const RtoR2::ParametricCurve_ptr &curve, PlotStyle plotStyle, Str label) {
        auto artist = Slab::New<ParametricCurve2DArtist>(curve, plotStyle);
        artist->setLabel(label);

        graph->addArtist(artist);

        return artist;
    }

    RtoRFunctionArtist_ptr
    Plotter::AddRtoRFunction(PlottingWindow_ptr graph, const RtoR::Function_ptr &function, PlotStyle plotStyle, Str label, Resolution samples) {
        auto artist = Slab::New<RtoRFunctionArtist>(function, plotStyle, samples);
        artist->setLabel(label);

        graph->addArtist(artist);

        return artist;
    }

    RtoRFunctionArtist_ptr
    Plotter::AddRtoRDiscreteFunction(PlottingWindow_ptr, const RtoR::DiscreteFunction_ptr &, PlotStyle, Str label) {
        NOT_IMPLEMENTED
    }

    R2toRFunctionArtist_ptr Plotter::AddR2toRFunction(PlottingWindow_ptr graph, const R2toR::DiscreteFunction_ptr &function, Str label) {
        auto artist = Slab::New<R2toRFunctionArtist>();
        artist->setLabel(label);

        artist->setFunction(function);

        graph->addArtist(artist);

        return artist;
    }

    HistoryArtist_ptr Plotter::AddRtoRHistory(PlottingWindow_ptr graph, const R2toR::DiscreteFunction_ptr &function, Str label) {
        auto artist = Slab::New<HistoryArtist>();
        artist->setLabel(label);

        artist->setFunction(function);

        graph->addArtist(artist);

        return artist;
    }

    R2SectionArtist_ptr
    Plotter::AddR2Section(PlottingWindow_ptr graph, const R2toR::Function_ptr &function, Str name) {
        auto artist = Slab::New<R2SectionArtist>();
        artist->setLabel(name);
        artist->setFunction(function);

        graph->addArtist(artist);

        return artist;
    }




} // Graphics