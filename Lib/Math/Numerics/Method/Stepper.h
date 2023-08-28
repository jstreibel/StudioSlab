#ifndef STEPPER_H
#define STEPPER_H

#include "Utils/Types.h"

class Stepper {
public:
    Stepper() = default;
    virtual ~Stepper() = default;

public:
    virtual void step(const Real &dt, size_t n_steps) = 0;
    virtual auto getCurrentState() const -> const void* = 0;
    virtual auto getSpaces() const -> DiscreteSpacePair = 0;
};

#endif /* STEPPER_NR_RK_H_ */
