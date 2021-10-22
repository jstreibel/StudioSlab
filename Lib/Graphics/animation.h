#ifndef ANIMATION_H
#define ANIMATION_H

#include <iostream>
#include <vector>
#include <cmath>

class Animation
{
public:
    Animation() {}

    Animation(double *var, double newVar, double faktor = 120.);

    void step(const double dt);

    void animateToValue(double newVar);

    double getVar() const;
    double getNewVar() const;

private:
    double *var;
    double targetValue;

private:
    double faktor;

};

#endif // ANIMATION_H
