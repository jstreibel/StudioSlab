//
// Created by joao on 10/11/22.
//

#ifndef STUDIOSLAB_GROWINGHOLE_H
#define STUDIOSLAB_GROWINGHOLE_H


#include "Fields/Mappings/R2toR/Model/R2toRFunction.h"

class GrowingHole : public R2toR::Function {
    Real t;
    Real height;
public:
    GrowingHole(Real height=1, Real t=.0);

    void setTime(Real t);
    Real operator()(Real2D x) const override;

    bool domainContainsPoint(Real2D x) const override;
};


#endif //STUDIOSLAB_GROWINGHOLE_H
