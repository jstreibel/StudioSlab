//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Studios/Util/MathTypes.h>

class Program {
public:
    virtual ~Program() = default;

    virtual void step(PosInt nSteps=1) = 0;

    /**
     * This is an alternative to calling start, step, finish
     */
    virtual void runFullIntegration() = 0;
};

#endif //V_SHAPE_PROGRAM_H