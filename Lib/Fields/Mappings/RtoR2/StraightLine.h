//
// Created by joao on 15/10/2019.
//

#ifndef V_SHAPE_STRAIGHTLINE_H
#define V_SHAPE_STRAIGHTLINE_H

#include <Fields/Mappings/R2toR/Model/Transform.h>
#include "ParametricCurve.h"

namespace RtoR2 {

    class StraightLine : public ParametricCurve {
    public:
        StraightLine() = default;
        StraightLine(Real2D x0, Real2D xf);
        StraightLine(const StraightLine& line);

        Real2D operator()(Real s) const override;

        friend StraightLine operator*(const Transform &T, const StraightLine &line ){
            Real2D x0 = line.x0;
            Real2D xf = line.x0 + line.r;

            return {T*x0, T*xf};
        }

        StraightLine &operator=(const StraightLine &rhs);

        Real2D getx0() const {return this->x0;}
        Real2D getr() const {return this->r;}

        Real length() const {return this->getr().norm();}
    private:
        Real2D x0, r;
    };

}


#endif //V_SHAPE_STRAIGHTLINE_H
