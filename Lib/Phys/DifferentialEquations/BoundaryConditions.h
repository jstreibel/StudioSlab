//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARYCONDITIONS_H
#define V_SHAPE_BOUNDARYCONDITIONS_H

#include "Common/Types.h"


namespace Base {
    template<class FUNCTION>
    class BoundaryConditions {
    public:
        virtual void apply(FUNCTION &toFunction, Real t) const = 0;
    };
}


#endif //V_SHAPE_BOUNDARYCONDITIONS_H
