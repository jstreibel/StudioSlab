#ifndef STEPPER_H
#define STEPPER_H

class Stepper{
public:
    Stepper() {}
    ~Stepper(){}

public:
    virtual void step(const double &dt, const size_t n_steps) = 0;
    virtual const void* getFieldState() const = 0;
    virtual DiscreteSpacePair getSpaces() const = 0;
};

#endif /* STEPPER_NR_RK_H_ */
