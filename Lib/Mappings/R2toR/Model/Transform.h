//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_TRANSFORM_H
#define V_SHAPE_TRANSFORM_H

#include "Math/Formalism/Categories.h"

class Transform {
public:
    virtual Real2D operator*(const Real2D &x) const = 0;
};

class Rotation : public Transform {
public:
    explicit Rotation(Real angleRad=.0);

    Real2D operator*(const Real2D &x) const override;

    static Rotation ByAngle(Real angleRad);

private:
    Real angle;
};


#endif //V_SHAPE_TRANSFORM_H
