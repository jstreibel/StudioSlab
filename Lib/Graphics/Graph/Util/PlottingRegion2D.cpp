//
// Created by joao on 6/8/24.
//

#include "PlottingRegion2D.h"
#include "Core/Backend/Modules/Animator/Animator.h"

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

        void PlottingRegion2D::animate_xMin(Real val) { Core::Animator::Set(*xMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_xMax(Real val) { Core::Animator::Set(*xMax, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMin(Real val) { Core::Animator::Set(*yMin, val, animationTimeSeconds);}
        void PlottingRegion2D::animate_yMax(Real val) { Core::Animator::Set(*yMax, val, animationTimeSeconds);}

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
            return Core::Animator::Contains(*xMin) ||
                   Core::Animator::Contains(*xMax) ||
                   Core::Animator::Contains(*yMin) ||
                   Core::Animator::Contains(*yMax);
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


    } // Slab
} // Graphics