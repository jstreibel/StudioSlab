//
// Created by joao on 3/25/23.
//

#ifndef STUDIOSLAB_STYLESANDCOLORSCHEMES_H
#define STUDIOSLAB_STYLESANDCOLORSCHEMES_H

#include "../PlottingUtils.h"

#ifndef TITLE_FONT
#define TITLE_FONT FONT_BITMAP_9
#endif
#ifndef TICK_FONT
#define TICK_FONT FONT_STROKE_ROMAN
#endif

namespace ColorSchemeDark {

    extern Color background,
                 graphTicksFont,
                 axisColor,
                 tickColor,
                 graphTitleFont;

    extern Color defaultGraphColor;

    extern Color graph1a, graph1b;
}

namespace ColorSchemeBWDark {
    extern Color background,
            graphTicksFont,
            axisColor,
            tickColor,
            graphTitleFont;

    extern Color defaultGraphColor;

    extern Color graph1a, graph1b;
}

namespace ColorSchemeTest {

    extern Color background,
                 graphTicksFont,
                 axisColor,
                 tickColor,
                 graphTitleFont;
}

namespace ColorScheme = ColorSchemeBWDark;

#endif //STUDIOSLAB_STYLESANDCOLORSCHEMES_H
