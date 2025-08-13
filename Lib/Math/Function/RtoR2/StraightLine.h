//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_STRAIGHTLINE_H
#define V_SHAPE_STRAIGHTLINE_H

#include "Math/Function/R2toR/Model/Transform.h"
#include "ParametricCurve.h"

namespace Slab::Math::RtoR2 {

    class StraightLine : public ParametricCurve {
        Real2D x0={0,0}, r={0,0};

    public:
        typedef std::shared_ptr<StraightLine> Ptr;

        StraightLine() = default;
        StraightLine(Real2D x0, Real2D xf, DevFloat sMin=0, DevFloat sMax=1);
        StraightLine(const StraightLine& line);


        Math::PointSet_ptr renderToPointSet(RenderingOptions options) override;

        static Ptr New(Real2D x0, Real2D xf);

        Real2D operator()(DevFloat s) const override;

        friend StraightLine operator*(const R2toR::Transform &T, const StraightLine &line);

        StraightLine &operator=(const StraightLine &rhs);

        Real2D getx0() const {return this->x0;}
        Real2D getr() const {return this->r;}

        Real2D& getx0() {return this->x0;}
        Real2D& getr() {return this->r;}

        DevFloat length() const {return this->getr().norm();}

    };

    DefinePointers(StraightLine)

}


#endif //V_SHAPE_STRAIGHTLINE_H
