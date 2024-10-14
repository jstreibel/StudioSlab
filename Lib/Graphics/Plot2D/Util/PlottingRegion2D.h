//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_PLOTTINGREGION2D_H
#define STUDIOSLAB_PLOTTINGREGION2D_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics {

    class PlottingRegion2D {
        Pointer<Real> xMin, xMax, yMin, yMax;
    public:
        explicit PlottingRegion2D(RectR);

        RectR getRect() const;

        bool isAnimating() const;

        void animate_xMin(Real);
        void animate_xMax(Real);
        void animate_yMin(Real);
        void animate_yMax(Real);

        void setLimits(Real xMin, Real xMax, Real yMin, Real yMax);

        void setReference_xMin(Pointer<Real>);
        void setReference_xMax(Pointer<Real>);
        void setReference_yMin(Pointer<Real>);
        void setReference_yMax(Pointer<Real>);

        auto getReference_xMin() const -> Pointer<Real>;
        auto getReference_xMax() const -> Pointer<Real>;
        auto getReference_yMin() const -> Pointer<Real>;
        auto getReference_yMax() const -> Pointer<Real>;

        Real getXMin() const;
        Real getXMax() const;
        Real getYMin() const;
        Real getYMax() const;

        Real height() const;
        Real width() const;
        Real xCenter() const;
        Real yCenter() const;

        void operator=(const RectR& rectR) {
            *xMin = rectR.xMin;
            *xMax = rectR.xMax;
            *yMin = rectR.yMin;
            *yMax = rectR.yMax;
        };

    };

} // Slab::Graphics

#endif //STUDIOSLAB_PLOTTINGREGION2D_H
