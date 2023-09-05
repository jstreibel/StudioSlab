//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLE_H
#define STUDIOSLAB_GROWINGHOLE_H


#include "Maps/R2toR/Model/R2toRFunction.h"

namespace R2toR {
    class GrowingHoleFunc : public R2toR::Function {
        Real t;
        Real height;
    public:
        GrowingHoleFunc(Real height = 1, Real t = .0);

        void setTime(Real t);

        Real operator()(Real2D x) const override;

        bool domainContainsPoint(Real2D x) const override;
    };

}


#endif //STUDIOSLAB_GROWINGHOLE_H
