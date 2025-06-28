//
// Created by joao on 17/10/2019.
//

#ifndef V_SHAPE_TRANSFORM_H
#define V_SHAPE_TRANSFORM_H

#include "Math/Formalism/Categories.h"


namespace Slab::Math::R2toR {


    class Transform {
    public:
        virtual Real2D operator*(const Real2D &x) const = 0;
    };

    class Rotation : public Transform {
    public:
        explicit Rotation(DevFloat angleRad = .0);

        Real2D operator*(const Real2D &x) const override;

        static Rotation ByAngle(DevFloat angleRad);

    private:
        DevFloat angle;
    };

    class Translation : public Transform {
        Real2D a;
    public:
        explicit Translation(Real2D d);

        Real2D operator*(const Real2D &x) const override;
    };

    class Scale : public Transform {
        Real2D s;
    public:
        explicit Scale(const Real2D &s);

        Real2D operator*(const Real2D &x) const override;
    };


}

#endif //V_SHAPE_TRANSFORM_H
