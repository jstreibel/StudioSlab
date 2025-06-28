//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLE_H
#define STUDIOSLAB_GROWINGHOLE_H


#include "Math/Function/R2toR/Model/R2toRFunction.h"

namespace Slab::Math::R2toR {
    class GrowingHoleFunc : public R2toR::Function {
        DevFloat t;
        DevFloat height;
    public:
        GrowingHoleFunc(DevFloat height = 1, DevFloat t = .0);

        void setTime(DevFloat t);

        DevFloat operator()(Real2D x) const override;

        bool domainContainsPoint(Real2D x) const override;
    };

}


#endif //STUDIOSLAB_GROWINGHOLE_H
