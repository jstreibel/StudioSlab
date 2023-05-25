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

namespace Styles {

    void Init();

    struct ColorScheme {
        ColorScheme(Color background, Color graphTicksFont, Color graphTitleFont, Color axisColor,
                    Color majorTickColor, std::vector<Color> plotColors);

        Color background;
        Color graphTicksFont;
        Color graphTitleFont;
        Color axisColor;
        Color majorTickColor;

        std::vector<Color> plotColors;
    };

    typedef std::shared_ptr<Styles::ColorScheme> ColorScheme_ptr;

    ColorScheme_ptr GetColorScheme();
}

#endif //STUDIOSLAB_STYLESANDCOLORSCHEMES_H
