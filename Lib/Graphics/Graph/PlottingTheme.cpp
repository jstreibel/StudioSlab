//
// Created by joao on 20/05/24.
//

#include "PlottingTheme.h"

#include <utility>

namespace Slab::Graphics {

    PlottingTheme::PlottingTheme(Color graphBackground,
                                 Color graphNumbersColor,
                                 Color graphTitleColor,
                                 Color axisColor,
                                 Color majorTickColor,
                                 PlotStyle XHairStyle,
                                 PlotStyle gridStyle,
                                 std::shared_ptr<Graphics::Writer> labelsWriter,
                                 std::shared_ptr<Graphics::Writer> ticksWriter,
                                 Vector<PlotStyle> plotStyles)
            : graphBackground(graphBackground), graphNumbersColor(graphNumbersColor), graphTitleColor(graphTitleColor),
              axisColor(axisColor), majorTickColor(majorTickColor), XHairStyle(XHairStyle), gridLines(gridStyle),
              labelsWriter(std::move(labelsWriter)), ticksWriter(std::move(ticksWriter)),
              funcPlotStyles(std::move(plotStyles)), vTickHeightinPixels(5), vAxisPaddingInPixels(80),
              hTickWidthInPixels(5), hAxisPaddingInPixels(30) {

    }

}