//
// Created by joao on 3/25/23.
//

#ifndef STUDIOSLAB_PLOTSTYLE_H
#define STUDIOSLAB_PLOTSTYLE_H

#include "Graphics/Utils.h"
#include "Graphics/Utils/Writer.h"
#include "Utils/Pointer.h"

#ifndef TITLE_FONT
#define TITLE_FONT FONT_BITMAP_9
#endif
#ifndef TICK_FONT
#define TICK_FONT FONT_STROKE_ROMAN
#endif

namespace Slab::Graphics {

    enum LinePrimitive {
        PlottingSolid=0,
        PlottingDotted,
        PlottingDashed,
        PlottingDotDashed,
        PlottingVerticalLinesWithCircles,

        Lines,
        LineStrip,
        LineLoop,

        Triangles,
        Quads,

        Points,

        __COUNT__
    };

    class PlotStyle {
        LinePrimitive primitive;

        GLint stippleFactor;
        GLshort stipplePattern;

    public:
        auto getPrimitive() const -> LinePrimitive;
        void setPrimitive(LinePrimitive);

        void setStippleFactor(GLint);
        auto getStippleFactor() const -> GLint;

        void setStipplePattern(GLshort);
        auto getStipplePattern() const -> GLshort;

        explicit PlotStyle(FColor color,
                           LinePrimitive primitive=PlottingSolid,
                           bool filled=true,
                           FColor fillColor=Nil,
                           float thickness=1.3);

        TPointer<PlotStyle> clone();
        
        PlotStyle permuteColors(bool odd=false) const;

        bool filled;
        FColor lineColor;
        FColor fillColor;
        float thickness;


    };
}

#endif //STUDIOSLAB_PLOTSTYLE_H
