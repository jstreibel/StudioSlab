//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_PLOTTINGTHEME_H
#define STUDIOSLAB_PLOTTINGTHEME_H

#include "Graphics/Styles/Colors.h"
#include "PlotStyle.h"

namespace Slab::Graphics {

    struct PlottingTheme {
        PlottingTheme(Color graphBackground,
                      Color graphNumbersColor,
                      Color graphTitleColor,
                      Color axisColor,
                      Color majorTickColor,
                      PlotStyle XHairStyle,
                      PlotStyle gridStyle,
                      std::shared_ptr <Graphics::Writer> labelsWriter,
                      std::shared_ptr <Graphics::Writer> ticksWriter,
                      Vector <PlotStyle> plotStyles);

        Color graphBackground;
        Color graphNumbersColor;
        Color graphTitleColor;
        Color axisColor;
        Color majorTickColor;
        PlotStyle XHairStyle;
        PlotStyle gridLines;

        std::shared_ptr <Graphics::Writer> labelsWriter;
        std::shared_ptr <Graphics::Writer> ticksWriter;

        Vector <PlotStyle> funcPlotStyles;

        Resolution vTickHeightinPixels;
        Resolution vAxisPaddingInPixels;

        Resolution hTickWidthInPixels;
        Resolution hAxisPaddingInPixels;
    };

    typedef Slab::Pointer <PlottingTheme> GraphTheme_ptr;

    GraphTheme_ptr NewTheme(const PlottingTheme &scheme);

}

#endif //STUDIOSLAB_PLOTTINGTHEME_H
