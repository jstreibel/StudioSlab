//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_STRAIGHTLINE_H
#define V_SHAPE_STRAIGHTLINE_H

#include "Mappings/R2toR/Model/Transform.h"
#include "ParametricCurve.h"

namespace RtoR2 {

    class StraightLine : public ParametricCurve {
        Real sMin, sMax;
        Real2D x0, r;

    public:
        typedef std::shared_ptr<StraightLine> Ptr;

        StraightLine() = default;
        StraightLine(Real2D x0, Real2D xf, Real sMin=0, Real sMax=1);
        StraightLine(const StraightLine& line);


        Spaces::PointSet::Ptr renderPointSet(PointSetRenderingOptions options) override;

        static Ptr New(Real2D x0, Real2D xf);



        Real2D operator()(Real s) const override;

        friend StraightLine operator*(const Transform &T, const StraightLine &line);

        StraightLine &operator=(const StraightLine &rhs);

        Real getDeltaS() const {return sMax-sMin;}
        Real getSMin() const {return sMin;}
        Real getSMax() const {return sMax;}
        Real2D getx0() const {return this->x0;}
        Real2D getr() const {return this->r;}

        Real length() const {return this->getr().norm();}

    };

}


#endif //V_SHAPE_STRAIGHTLINE_H
