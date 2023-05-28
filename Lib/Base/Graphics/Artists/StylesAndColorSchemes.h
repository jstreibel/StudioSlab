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

    enum Trace {
        Solid,
        Dotted,
        Dashed,
        DotDashed
    };

    const Color Nil = Color(-1,-1,-1,-1);


    struct PlotStyle {
        PlotStyle(Color color, Trace trace=Solid, bool filled=true, Color lineFill=Nil, float lineWidth=1.3);

        Color lineColor;
        Trace trace;
        bool filled;
        Color fillColor;
        float lineWidth;

        unsigned stippleFactor, stipplePattern;
    };

    struct StyleScheme {
        StyleScheme(Color background, Color graphTicksFont, Color graphTitleFont, Color axisColor,
                    Color majorTickColor, std::vector<PlotStyle> plotStyles);

        Color background;
        Color graphTicksFont;
        Color graphTitleFont;
        Color axisColor;
        Color majorTickColor;

        std::vector<PlotStyle> funcPlotStyles;
    };

    typedef std::shared_ptr<Styles::StyleScheme> ColorScheme_ptr;

    ColorScheme_ptr NewScheme(StyleScheme scheme);

    ColorScheme_ptr GetColorScheme();
}

#endif //STUDIOSLAB_STYLESANDCOLORSCHEMES_H
