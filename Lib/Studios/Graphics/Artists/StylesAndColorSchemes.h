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

    extern std::vector<Color> graphs;
}

namespace ColorSchemeBWDark {
    extern Color background,
            graphTicksFont,
            axisColor,
            tickColor,
            graphTitleFont;

    extern Color defaultGraphColor;

    extern std::vector<Color> graphs;
}

namespace ColorSchemePrint {
    extern Color background,
            graphTicksFont,
            axisColor,
            tickColor,
            graphTitleFont;

    extern Color defaultGraphColor;

    extern std::vector<Color> graphs;
}

namespace ColorSchemeTest {

    extern Color background,
                 graphTicksFont,
                 axisColor,
                 tickColor,
                 graphTitleFont;
}

namespace ColorScheme = ColorSchemeDark;

#endif //STUDIOSLAB_STYLESANDCOLORSCHEMES_H
