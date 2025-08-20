//
// Created by joao on 21/05/24.
//

#include "Plotter.h"

namespace Slab::Graphics {

    FPlotter::FPlotter(TPointer<Graphics::FPlot2DWindow> win) : plottingWindow(win)
    {

    }

    PointSetArtist_ptr FPlotter::addPointSet(const Math::PointSet_ptr &data, PlotStyle style, Str name, bool affectsGraphRanges, int zOrder) {
        return FPlotter::AddPointSet(plottingWindow, data, style, std::move(name), affectsGraphRanges, zOrder);
    }

    ParametricCurve2DArtist_ptr FPlotter::addCurve(const RtoR2::ParametricCurve_ptr &data, PlotStyle style, const Str& name, int zOrder) {
        return FPlotter::AddCurve(plottingWindow, data, style, name, zOrder);
    }

    RtoRFunctionArtist_ptr
    FPlotter::addRtoRFunction(const RtoR::Function_ptr &data, PlotStyle style, Str name, Resolution samples, int zOrder) {
        return FPlotter::AddRtoRFunction(plottingWindow, data, style, std::move(name), samples, zOrder);
    }

    RtoRFunctionArtist_ptr FPlotter::addRtoRNumericFunction(const RtoR::NumericFunction_ptr &data, PlotStyle style, Str name, int zOrder) {
        return FPlotter::AddRtoRNumericFunction(plottingWindow, data, style, std::move(name), zOrder);
    }

    R2toRFunctionArtist_ptr FPlotter::addR2toRFunction(const R2toR::FNumericFunction_constptr &data, Str name, int zOrder) {
        return FPlotter::AddR2toRFunction(plottingWindow, data, std::move(name), zOrder);
    }

    HistoryArtist_ptr FPlotter::addRtoRHistory(const R2toR::FNumericFunction_constptr &data, Str name, int zOrder) {
        return FPlotter::AddRtoRHistory(plottingWindow, data, std::move(name), zOrder);
    }

    R2SectionArtist_ptr FPlotter::addR2Section(const R2toR::Function_constptr &data, Str name, int zOrder) {
        return AddR2Section(plottingWindow, data, std::move(name), zOrder);
    }

    PointSetArtist_ptr
    FPlotter::AddPointSet(const TPointer<FPlot2DWindow>& graph, const Math::PointSet_ptr &pointSet, PlotStyle plotStyle, Str label, bool affectsGraphRanges, int zOrder) {
        auto artist = Slab::New<PointSetArtist>(pointSet, plotStyle);
        artist->SetLabel(std::move(label));

        artist->SetAffectGraphRanges(affectsGraphRanges);

        graph->AddArtist(artist, zOrder);

        return artist;
    }

    ParametricCurve2DArtist_ptr
    FPlotter::AddCurve(const TPointer<FPlot2DWindow>& graph, const RtoR2::ParametricCurve_ptr &curve, PlotStyle plotStyle, Str label, int zOrder) {
        auto artist = Slab::New<ParametricCurve2DArtist>(curve, plotStyle);
        artist->SetLabel(std::move(label));

        graph->AddArtist(artist, zOrder);

        return artist;
    }

    RtoRFunctionArtist_ptr
    FPlotter::AddRtoRFunction(const TPointer<FPlot2DWindow>& graph, const RtoR::Function_ptr &function, PlotStyle plotStyle, Str label, Resolution samples, int zOrder) {
        auto artist = Slab::New<RtoRFunctionArtist>(function, plotStyle, samples);
        artist->SetLabel(std::move(label));

        graph->AddArtist(artist, zOrder);

        return artist;
    }

    RtoRFunctionArtist_ptr
    FPlotter::AddRtoRNumericFunction(TPointer<FPlot2DWindow>, const RtoR::NumericFunction_ptr &, PlotStyle, Str label, int zOrder) {
        NOT_IMPLEMENTED
    }

    R2toRFunctionArtist_ptr FPlotter::AddR2toRFunction(const TPointer<FPlot2DWindow>& graph, const R2toR::FNumericFunction_constptr &function, Str name, const int zOrder) {
        auto artist = Slab::New<R2toRFunctionArtist>();
        artist->SetLabel(std::move(name));
        artist->setFunction(function);
        graph->AddArtist(artist, zOrder);

        fix cmap_painter = DynamicPointerCast<Colormap1DPainter>(artist->getPainter("Colormap"));
        fix colorbar = cmap_painter->getColorBarArtist();
        graph->AddArtist(colorbar, 10);

        return artist;
    }

    HistoryArtist_ptr FPlotter::AddRtoRHistory(const TPointer<FPlot2DWindow>& graph, const R2toR::FNumericFunction_constptr &function, Str label, int zOrder) {
        auto artist = Slab::New<HistoryArtist>();
        artist->SetLabel(std::move(label));

        artist->setFunction(function);

        graph->AddArtist(artist, zOrder);

        return artist;
    }

    R2SectionArtist_ptr
    FPlotter::AddR2Section(const TPointer<FPlot2DWindow>& graph, const R2toR::Function_constptr &function, Str name, int zOrder) {
        auto artist = Slab::New<R2SectionArtist>();
        artist->SetLabel(std::move(name));
        artist->setFunction(function);

        graph->AddArtist(artist, zOrder);

        return artist;
    }


} // Graphics