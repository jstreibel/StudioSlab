//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_PLOTTINGTHEME_H
#define STUDIOSLAB_PLOTTINGTHEME_H

#include "Graphics/Styles/Colors.h"
#include "PlotStyle.h"

namespace Slab::Graphics {

    struct PlottingTheme {
        PlottingTheme(FColor graphBackground,
                      FColor graphNumbersColor,
                      FColor graphTitleColor,
                      FColor axisColor,
                      FColor majorTickColor,
                      PlotStyle XHairStyle,
                      PlotStyle majorGridStyle,
                      PlotStyle minorGridStyle,
                      std::shared_ptr <Graphics::FWriter> labelsWriter,
                      std::shared_ptr <Graphics::FWriter> ticksWriter,
                      Vector <PlotStyle> plotStyles);

        FColor graphBackground;
        FColor graphNumbersColor;
        FColor graphTitleColor;
        FColor axisColor;
        FColor majorTickColor;
        PlotStyle XHairStyle;
        PlotStyle majorGridLines;
        PlotStyle minorGridLines;

        std::shared_ptr <Graphics::FWriter> LabelsWriter;
        std::shared_ptr <Graphics::FWriter> TicksWriter;

        Vector <PlotStyle> FuncPlotStyles;

        Resolution vTickHeightinPixels;
        Resolution vAxisPaddingInPixels;

        Resolution hTickWidthInPixels;
        Resolution hAxisPaddingInPixels;
    };

    typedef Slab::TPointer <PlottingTheme> GraphTheme_ptr;

}

#endif //STUDIOSLAB_PLOTTINGTHEME_H
