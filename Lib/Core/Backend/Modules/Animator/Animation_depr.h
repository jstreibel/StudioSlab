#ifndef ANIMATION_H
#define ANIMATION_H

#include "Utils/Types.h"

#include <iostream>
#include <vector>
#include <cmath>

class Animation_depr
{
public:
    Animation_depr() {}

    Animation_depr(Real *var, Real newVar, Real faktor = 120.);

    void step(const Real dt);

    void animateToValue(Real newVar);

    Real getVar() const;
    Real getNewVar() const;

private:
    Real *var;
    Real targetValue;

private:
    Real faktor;

};

#endif // ANIMATION_H
