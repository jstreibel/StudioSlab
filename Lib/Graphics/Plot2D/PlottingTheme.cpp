//
// Created by joao on 20/05/24.
//

#include "PlottingTheme.h"

#include <utility>

namespace Slab::Graphics {

    PlottingTheme::PlottingTheme(FColor graphBackground,
                                 FColor graphNumbersColor,
                                 FColor graphTitleColor,
                                 FColor axisColor,
                                 FColor majorTickColor,
                                 PlotStyle XHairStyle,
                                 PlotStyle majorGridStyle,
                                 PlotStyle minorGridStyle,
                                 std::shared_ptr<Graphics::FWriter> labelsWriter,
                                 std::shared_ptr<Graphics::FWriter> ticksWriter,
                                 Vector<PlotStyle> plotStyles)
            : graphBackground(graphBackground), graphNumbersColor(graphNumbersColor), graphTitleColor(graphTitleColor),
              axisColor(axisColor), majorTickColor(majorTickColor), XHairStyle(XHairStyle),
              majorGridLines(majorGridStyle), minorGridLines(minorGridStyle),
              LabelsWriter(std::move(labelsWriter)), TicksWriter(std::move(ticksWriter)),
              FuncPlotStyles(std::move(plotStyles)), vTickHeightinPixels(5), vAxisPaddingInPixels(80),
              hTickWidthInPixels(5), hAxisPaddingInPixels(30) {

    }

}