//
// Created by joao on 6/8/24.
//

#include "PlottingRegion2D.h"
#include "Graphics/Modules/Animator/Animator.h"

namespace Slab {
    namespace Graphics {

        Real animationTimeSeconds = 0.2;

        PlottingRegion2D::PlottingRegion2D(RectR rect) {
            xMin = New<Real>(rect.xMin);
            xMax = New<Real>(rect.xMax);
            yMin = New<Real>(rect.yMin);
            yMax = New<Real>(rect.xMax);
        }

        RectR PlottingRegion2D::getRect() const {return {*xMin, *xMax, *yMin, *yMax}; }

        void PlottingRegion2D::animate_xMin(Real val) { Graphics::Animator::Set(*xMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_xMax(Real val) { Graphics::Animator::Set(*xMax, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMin(Real val) { Graphics::Animator::Set(*yMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMax(Real val) { Graphics::Animator::Set(*yMax, val, animationTimeSeconds);}

        void PlottingRegion2D::setReference_xMin(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(0.0);
            xMin = ref;
        }
        void PlottingRegion2D::setReference_xMax(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(1.0);
            xMax = ref;
        }
        void PlottingRegion2D::setReference_yMin(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(0.0);
            yMin = ref;
        }
        void PlottingRegion2D::setReference_yMax(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(1.0);
            yMax = ref;
        }

        auto PlottingRegion2D::getReference_xMin() const -> Pointer<Real> { return xMin; }
        auto PlottingRegion2D::getReference_xMax() const -> Pointer<Real> { return xMax; }
        auto PlottingRegion2D::getReference_yMin() const -> Pointer<Real> { return yMin; }
        auto PlottingRegion2D::getReference_yMax() const -> Pointer<Real> { return yMax; }

        Real PlottingRegion2D::width()   const { return *xMax - *xMin; }
        Real PlottingRegion2D::height()  const { return *yMax - *yMin; }
        Real PlottingRegion2D::xCenter() const { return (*xMax + *xMin) * .5; }
        Real PlottingRegion2D::yCenter() const { return (*yMax + *yMin) * .5; }

        bool PlottingRegion2D::isAnimating() const {
            return Animator::Contains(*xMin) ||
                   Animator::Contains(*xMax) ||
                   Animator::Contains(*yMin) ||
                   Animator::Contains(*yMax);
        }

        Real PlottingRegion2D::getXMin() const {
            return *xMin;
        }

        Real PlottingRegion2D::getXMax() const {
            return *xMax;
        }

        Real PlottingRegion2D::getYMin() const {
            return *yMin;
        }

        Real PlottingRegion2D::getYMax() const {
            return *yMax;
        }

        void PlottingRegion2D::setLimits(Real x_min, Real x_max, Real y_min, Real y_max) {
            *xMin = x_min;
            *xMax = x_max;
            *yMin = y_min;
            *yMax = y_max;
        }

        void PlottingRegion2D::set_x_limits(Real x_min, Real x_max) {
            *xMin = x_min;
            *xMax = x_max;
        }

        void PlottingRegion2D::set_y_limits(Real y_min, Real y_max) {
            *yMin = y_min;
            *yMax = y_max;
        }


    } // Slab
} // Graphics