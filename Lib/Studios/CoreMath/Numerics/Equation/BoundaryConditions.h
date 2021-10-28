//
// Created by joao on 05/09/2019.
//

#ifndef V_SHAPE_BOUNDARYCONDITIONS_H
#define V_SHAPE_BOUNDARYCONDITIONS_H

#include <Studios/Util/Typedefs.h>


namespace Base {
    template<class FIELD_STATE_TYPE>
    class BoundaryConditions {
    public:
        virtual void apply(FIELD_STATE_TYPE &fieldState, Real t) const = 0;
    };
}


#endif //V_SHAPE_BOUNDARYCONDITIONS_H
