//
// Created by joao on 6/8/24.
//

#include "PlottingRegion2D.h"
#include "Graphics/Modules/Animator/Animator.h"

namespace Slab {
    namespace Graphics {

        DevFloat animationTimeSeconds = 0.2;

        PlottingRegion2D::PlottingRegion2D(RectR rect) {
            xMin = New<DevFloat>(rect.xMin);
            xMax = New<DevFloat>(rect.xMax);
            yMin = New<DevFloat>(rect.yMin);
            yMax = New<DevFloat>(rect.xMax);
        }

        RectR PlottingRegion2D::getRect() const {return {*xMin, *xMax, *yMin, *yMax}; }

        void PlottingRegion2D::animate_xMin(DevFloat val) { Graphics::Animator::Set(*xMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_xMax(DevFloat val) { Graphics::Animator::Set(*xMax, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMin(DevFloat val) { Graphics::Animator::Set(*yMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMax(DevFloat val) { Graphics::Animator::Set(*yMax, val, animationTimeSeconds);}

        void PlottingRegion2D::setReference_xMin(TPointer<DevFloat> ref) {
            if(ref == nullptr) ref = New<DevFloat>(0.0);
            xMin = ref;
        }
        void PlottingRegion2D::setReference_xMax(TPointer<DevFloat> ref) {
            if(ref == nullptr) ref = New<DevFloat>(1.0);
            xMax = ref;
        }
        void PlottingRegion2D::setReference_yMin(TPointer<DevFloat> ref) {
            if(ref == nullptr) ref = New<DevFloat>(0.0);
            yMin = ref;
        }
        void PlottingRegion2D::setReference_yMax(TPointer<DevFloat> ref) {
            if(ref == nullptr) ref = New<DevFloat>(1.0);
            yMax = ref;
        }

        auto PlottingRegion2D::getReference_xMin() const -> TPointer<DevFloat> { return xMin; }
        auto PlottingRegion2D::getReference_xMax() const -> TPointer<DevFloat> { return xMax; }
        auto PlottingRegion2D::getReference_yMin() const -> TPointer<DevFloat> { return yMin; }
        auto PlottingRegion2D::getReference_yMax() const -> TPointer<DevFloat> { return yMax; }

        DevFloat PlottingRegion2D::width()   const { return *xMax - *xMin; }
        DevFloat PlottingRegion2D::height()  const { return *yMax - *yMin; }
        DevFloat PlottingRegion2D::xCenter() const { return (*xMax + *xMin) * .5; }
        DevFloat PlottingRegion2D::yCenter() const { return (*yMax + *yMin) * .5; }

        bool PlottingRegion2D::isAnimating() const {
            return Animator::Contains(*xMin) ||
                   Animator::Contains(*xMax) ||
                   Animator::Contains(*yMin) ||
                   Animator::Contains(*yMax);
        }

        DevFloat PlottingRegion2D::getXMin() const {
            return *xMin;
        }

        DevFloat PlottingRegion2D::getXMax() const {
            return *xMax;
        }

        DevFloat PlottingRegion2D::getYMin() const {
            return *yMin;
        }

        DevFloat PlottingRegion2D::getYMax() const {
            return *yMax;
        }

        void PlottingRegion2D::setLimits(DevFloat x_min, DevFloat x_max, DevFloat y_min, DevFloat y_max) {
            *xMin = x_min;
            *xMax = x_max;
            *yMin = y_min;
            *yMax = y_max;
        }

        void PlottingRegion2D::set_x_limits(DevFloat x_min, DevFloat x_max) {
            *xMin = x_min;
            *xMax = x_max;
        }

        void PlottingRegion2D::set_y_limits(DevFloat y_min, DevFloat y_max) {
            *yMin = y_min;
            *yMax = y_max;
        }


    } // Slab
} // Graphics