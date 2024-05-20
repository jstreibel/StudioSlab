//
// Created by joao on 20/05/24.
//

#ifndef STUDIOSLAB_GRAPHTHEME_H
#define STUDIOSLAB_GRAPHTHEME_H

#include "Graphics/Styles/Colors.h"
#include "PlotStyle.h"

namespace Graphics {

    struct GraphTheme {
        GraphTheme(Color graphBackground,
                   Color graphNumbersColor,
                   Color graphTitleColor,
                   Color axisColor,
                   Color majorTickColor,
                   PlotStyle XHairStyle,
                   PlotStyle gridStyle,
                   std::shared_ptr <Graphics::Writer> labelsWriter,
                   std::shared_ptr <Graphics::Writer> ticksWriter,
                   std::vector <PlotStyle> plotStyles);

        Color graphBackground;
        Color graphNumbersColor;
        Color graphTitleColor;
        Color axisColor;
        Color majorTickColor;
        PlotStyle XHairStyle;
        PlotStyle gridLines;

        std::shared_ptr <Graphics::Writer> labelsWriter;
        std::shared_ptr <Graphics::Writer> ticksWriter;

        std::vector <PlotStyle> funcPlotStyles;

        Resolution vTickHeightinPixels;
        Resolution vAxisPaddingInPixels;

        Resolution hTickWidthInPixels;
        Resolution hAxisPaddingInPixels;
    };

    typedef Slab::Pointer <GraphTheme> GraphTheme_ptr;

    GraphTheme_ptr NewTheme(const GraphTheme &scheme);

}

#endif //STUDIOSLAB_GRAPHTHEME_H
