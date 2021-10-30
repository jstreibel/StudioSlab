#ifndef STEPPER_H
#define STEPPER_H

class Method{
public:
    Method() = default;
    virtual ~Method() = default;

public:
    virtual void step(const double &dt, size_t n_steps) = 0;
    virtual auto getFieldState() const -> const void* = 0;
    virtual auto getSpaces() const -> DiscreteSpacePair = 0;
};

#endif /* STEPPER_NR_RK_H_ */
