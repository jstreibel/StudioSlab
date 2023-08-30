//
// Created by joao on 3/25/23.
//

#ifndef STUDIOSLAB_STYLESANDCOLORSCHEMES_H
#define STUDIOSLAB_STYLESANDCOLORSCHEMES_H

#include "Core/Graphics/Utils.h"

#ifndef TITLE_FONT
#define TITLE_FONT FONT_BITMAP_9
#endif
#ifndef TICK_FONT
#define TICK_FONT FONT_STROKE_ROMAN
#endif

namespace Styles {

    void Init();

    enum Primitive {
        SolidLine,
        DottedLine,
        DashedLine,
        DotDashedLine,

        Point
    };

    const Color Nil = Color(-1,-1,-1,-1);


    struct PlotStyle {
        explicit PlotStyle(Color color, Primitive primitive=SolidLine, bool filled=true, Color lineFill=Nil, float lineWidth=1.3);

        PlotStyle permuteColors(bool odd=false);

        Color lineColor;
        Primitive primitive;
        bool filled;
        Color fillColor;
        float thickness;

        GLint stippleFactor;
        GLshort stipplePattern;
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

    ColorScheme_ptr NewScheme(const StyleScheme& scheme);

    ColorScheme_ptr GetColorScheme();
}

#endif //STUDIOSLAB_STYLESANDCOLORSCHEMES_H