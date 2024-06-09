//
// Created by joao on 6/8/24.
//

#include "PlotRegion2D.h"
#include "Core/Backend/Modules/Animator/Animator.h"

namespace Slab {
    namespace Graphics {

        Real animationTimeSeconds = 0.2;

        PlotRegion2D::PlotRegion2D(RectR rect) {
            xMin = New<Real>(rect.xMin);
            xMax = New<Real>(rect.xMax);
            yMin = New<Real>(rect.yMin);
            yMax = New<Real>(rect.xMax);
        }

        RectR PlotRegion2D::getRect() const {return {*xMin, *xMax, *yMin, *yMax}; }

        void PlotRegion2D::animate_xMin(Real val) { Core::Animator::Set(*xMin, val, animationTimeSeconds);}
        void PlotRegion2D::animate_xMax(Real val) { Core::Animator::Set(*xMax, val, animationTimeSeconds);}
        void PlotRegion2D::animate_yMin(Real val) { Core::Animator::Set(*yMin, val, animationTimeSeconds);}
        void PlotRegion2D::animate_yMax(Real val) { Core::Animator::Set(*yMax, val, animationTimeSeconds);}

        void PlotRegion2D::setReference_xMin(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(0.0);
            xMin = ref;
        }
        void PlotRegion2D::setReference_xMax(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(1.0);
            xMax = ref;
        }
        void PlotRegion2D::setReference_yMin(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(0.0);
            yMin = ref;
        }
        void PlotRegion2D::setReference_yMax(Pointer<Real> ref) {
            if(ref == nullptr) ref = New<Real>(1.0);
            yMax = ref;
        }

        auto PlotRegion2D::getReference_xMin() const -> Pointer<Real> { return xMin; }
        auto PlotRegion2D::getReference_xMax() const -> Pointer<Real> { return xMax; }
        auto PlotRegion2D::getReference_yMin() const -> Pointer<Real> { return yMin; }
        auto PlotRegion2D::getReference_yMax() const -> Pointer<Real> { return yMax; }

        Real PlotRegion2D::width()   const { return  *xMax - *xMin; }
        Real PlotRegion2D::height()  const { return  *yMax - *yMin; }
        Real PlotRegion2D::xCenter() const { return (*xMax + *xMin)*.5; }
        Real PlotRegion2D::yCenter() const { return (*yMax + *yMin)*.5; }

        bool PlotRegion2D::isAnimating() const {
            return Core::Animator::Contains(*xMin) ||
                   Core::Animator::Contains(*xMax) ||
                   Core::Animator::Contains(*yMin) ||
                   Core::Animator::Contains(*yMax);
        }

        Real PlotRegion2D::getXMin() const {
            return *xMin;
        }

        Real PlotRegion2D::getXMax() const {
            return *xMax;
        }

        Real PlotRegion2D::getYMin() const {
            return *yMin;
        }

        Real PlotRegion2D::getYMax() const {
            return *yMax;
        }


    } // Slab
} // Graphics