//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SPACE_H
#define STUDIOSLAB_SPACE_H

#include "Measure.h"

class Space {
public:
    typedef std::shared_ptr<Space> Ptr;

    explicit Space(PosInt dim);

    virtual auto getMeasure() const -> const Measure = 0;

    const PosInt dim;
};


#endif //STUDIOSLAB_SPACE_H
