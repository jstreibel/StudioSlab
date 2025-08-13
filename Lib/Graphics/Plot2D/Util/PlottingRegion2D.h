//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_PLOTTINGREGION2D_H
#define STUDIOSLAB_PLOTTINGREGION2D_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics {

    class PlottingRegion2D {
        TPointer<DevFloat> xMin, xMax, yMin, yMax;
    public:
        explicit PlottingRegion2D(RectR);

        RectR getRect() const;

        bool isAnimating() const;

        void animate_xMin(DevFloat);
        void animate_xMax(DevFloat);
        void animate_yMin(DevFloat);
        void animate_yMax(DevFloat);

        void setLimits(DevFloat xMin, DevFloat xMax, DevFloat yMin, DevFloat yMax);
        void set_x_limits(DevFloat x_min, DevFloat x_max);
        void set_y_limits(DevFloat y_min, DevFloat y_max);

        void setReference_xMin(TPointer<DevFloat>);
        void setReference_xMax(TPointer<DevFloat>);
        void setReference_yMin(TPointer<DevFloat>);
        void setReference_yMax(TPointer<DevFloat>);

        auto getReference_xMin() const -> TPointer<DevFloat>;
        auto getReference_xMax() const -> TPointer<DevFloat>;
        auto getReference_yMin() const -> TPointer<DevFloat>;
        auto getReference_yMax() const -> TPointer<DevFloat>;

        DevFloat getXMin() const;
        DevFloat getXMax() const;
        DevFloat getYMin() const;
        DevFloat getYMax() const;

        DevFloat height() const;
        DevFloat width() const;
        DevFloat xCenter() const;
        DevFloat yCenter() const;

        void operator=(const RectR& rectR) {
            *xMin = rectR.xMin;
            *xMax = rectR.xMax;
            *yMin = rectR.yMin;
            *yMax = rectR.yMax;
        };

    };

} // Slab::Graphics

#endif //STUDIOSLAB_PLOTTINGREGION2D_H
