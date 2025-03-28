//
// Created by joao on 21/05/24.
//

#include "Plotter.h"

namespace Slab::Graphics {

    Plotter::Plotter(Pointer<Graphics::Plot2DWindow> win) : Core::Interface("Plot2D:Plotter"), plottingWindow(win) {}

    PointSetArtist_ptr Plotter::addPointSet(const Math::PointSet_ptr &data, PlotStyle style, Str name, bool affectsGraphRanges, int zOrder) {
        return Plotter::AddPointSet(plottingWindow, data, style, std::move(name), affectsGraphRanges, zOrder);
    }

    ParametricCurve2DArtist_ptr Plotter::addCurve(const RtoR2::ParametricCurve_ptr &data, PlotStyle style, const Str& name, int zOrder) {
        return Plotter::AddCurve(plottingWindow, data, style, name, zOrder);
    }

    RtoRFunctionArtist_ptr
    Plotter::addRtoRFunction(const RtoR::Function_ptr &data, PlotStyle style, Str name, Resolution samples, int zOrder) {
        return Plotter::AddRtoRFunction(plottingWindow, data, style, std::move(name), samples, zOrder);
    }

    RtoRFunctionArtist_ptr Plotter::addRtoRNumericFunction(const RtoR::NumericFunction_ptr &data, PlotStyle style, Str name, int zOrder) {
        return Plotter::AddRtoRNumericFunction(plottingWindow, data, style, std::move(name), zOrder);
    }

    R2toRFunctionArtist_ptr Plotter::addR2toRFunction(const R2toR::NumericFunction_constptr &data, Str name, int zOrder) {
        return Plotter::AddR2toRFunction(plottingWindow, data, std::move(name), zOrder);
    }

    HistoryArtist_ptr Plotter::addRtoRHistory(const R2toR::NumericFunction_constptr &data, Str name, int zOrder) {
        return Plotter::AddRtoRHistory(plottingWindow, data, std::move(name), zOrder);
    }

    R2SectionArtist_ptr Plotter::addR2Section(const R2toR::Function_constptr &data, Str name, int zOrder) {
        return AddR2Section(plottingWindow, data, std::move(name), zOrder);
    }

    PointSetArtist_ptr
    Plotter::AddPointSet(const Pointer<Plot2DWindow>& graph, const Math::PointSet_ptr &pointSet, PlotStyle plotStyle, Str label, bool affectsGraphRanges, int zOrder) {
        auto artist = Slab::New<PointSetArtist>(pointSet, plotStyle);
        artist->setLabel(std::move(label));

        artist->setAffectGraphRanges(affectsGraphRanges);

        graph->addArtist(artist, zOrder);

        return artist;
    }

    ParametricCurve2DArtist_ptr
    Plotter::AddCurve(const Pointer<Plot2DWindow>& graph, const RtoR2::ParametricCurve_ptr &curve, PlotStyle plotStyle, Str label, int zOrder) {
        auto artist = Slab::New<ParametricCurve2DArtist>(curve, plotStyle);
        artist->setLabel(std::move(label));

        graph->addArtist(artist, zOrder);

        return artist;
    }

    RtoRFunctionArtist_ptr
    Plotter::AddRtoRFunction(const Pointer<Plot2DWindow>& graph, const RtoR::Function_ptr &function, PlotStyle plotStyle, Str label, Resolution samples, int zOrder) {
        auto artist = Slab::New<RtoRFunctionArtist>(function, plotStyle, samples);
        artist->setLabel(std::move(label));

        graph->addArtist(artist, zOrder);

        return artist;
    }

    RtoRFunctionArtist_ptr
    Plotter::AddRtoRNumericFunction(Pointer<Plot2DWindow>, const RtoR::NumericFunction_ptr &, PlotStyle, Str label, int zOrder) {
        NOT_IMPLEMENTED
    }

    R2toRFunctionArtist_ptr Plotter::AddR2toRFunction(const Pointer<Plot2DWindow>& graph, const R2toR::NumericFunction_constptr &function, Str name, const int zOrder) {
        auto artist = Slab::New<R2toRFunctionArtist>();
        artist->setLabel(std::move(name));
        artist->setFunction(function);
        graph->addArtist(artist, zOrder);

        fix cmap_painter = DynamicPointerCast<Colormap1DPainter>(artist->getPainter("Colormap"));
        fix colorbar = cmap_painter->getColorBarArtist();
        graph->addArtist(colorbar, 10);

        return artist;
    }

    HistoryArtist_ptr Plotter::AddRtoRHistory(const Pointer<Plot2DWindow>& graph, const R2toR::NumericFunction_constptr &function, Str label, int zOrder) {
        auto artist = Slab::New<HistoryArtist>();
        artist->setLabel(std::move(label));

        artist->setFunction(function);

        graph->addArtist(artist, zOrder);

        return artist;
    }

    R2SectionArtist_ptr
    Plotter::AddR2Section(const Pointer<Plot2DWindow>& graph, const R2toR::Function_constptr &function, Str name, int zOrder) {
        auto artist = Slab::New<R2SectionArtist>();
        artist->setLabel(std::move(name));
        artist->setFunction(function);

        graph->addArtist(artist, zOrder);

        return artist;
    }

    Core::Message Plotter::sendRequest(Core::Request request) {
        return Interface::sendRequest(request);
    }


} // Graphics