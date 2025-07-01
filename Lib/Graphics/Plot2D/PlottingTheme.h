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
                      std::shared_ptr <Graphics::Writer> labelsWriter,
                      std::shared_ptr <Graphics::Writer> ticksWriter,
                      Vector <PlotStyle> plotStyles);

        FColor graphBackground;
        FColor graphNumbersColor;
        FColor graphTitleColor;
        FColor axisColor;
        FColor majorTickColor;
        PlotStyle XHairStyle;
        PlotStyle majorGridLines;
        PlotStyle minorGridLines;

        std::shared_ptr <Graphics::Writer> LabelsWriter;
        std::shared_ptr <Graphics::Writer> TicksWriter;

        Vector <PlotStyle> FuncPlotStyles;

        Resolution vTickHeightinPixels;
        Resolution vAxisPaddingInPixels;

        Resolution hTickWidthInPixels;
        Resolution hAxisPaddingInPixels;
    };

    typedef Slab::Pointer <PlottingTheme> GraphTheme_ptr;

}

#endif //STUDIOSLAB_PLOTTINGTHEME_H
