//
// Created by joao on 3/25/23.
//

#ifndef STUDIOSLAB_PLOTSTYLE_H
#define STUDIOSLAB_PLOTSTYLE_H

#include "Graphics/Utils.h"
#include "Graphics/OpenGL/Writer.h"
#include "Utils/Pointer.h"

#ifndef TITLE_FONT
#define TITLE_FONT FONT_BITMAP_9
#endif
#ifndef TICK_FONT
#define TICK_FONT FONT_STROKE_ROMAN
#endif

namespace Slab::Graphics {

    enum LinePrimitive {
        Solid=0,
        Dotted,
        Dashed,
        DotDashed,

        VerticalLines,

        Lines,

        Point,

        __COUNT__
    };

    struct PlotStyle {
        explicit PlotStyle(Color color,
                           LinePrimitive primitive=Solid,
                           bool filled=true,
                           Color fillColor=Nil,
                           float thickness=1.3);

        Pointer<PlotStyle> clone();
        
        PlotStyle permuteColors(bool odd=false) const;

        Color lineColor;
        LinePrimitive primitive;

        bool filled;
        Color fillColor;
        float thickness;

        GLint stippleFactor;
        GLshort stipplePattern;
    };
}

#endif //STUDIOSLAB_PLOTSTYLE_H
