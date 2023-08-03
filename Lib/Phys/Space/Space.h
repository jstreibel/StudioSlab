//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SPACE_H
#define STUDIOSLAB_SPACE_H

#include "Measure.h"

class Space {
    PosInt dim;

public:
    typedef std::shared_ptr<Space> Ptr;

    explicit Space(PosInt dim);
    Space(const Space &);

    virtual auto getMeasure() const -> const Measure = 0;
    virtual auto getDim() const -> PosInt;

};


#endif //STUDIOSLAB_SPACE_H
