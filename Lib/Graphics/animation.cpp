#include "animation.h"

Animation::Animation(double *var, double newVar, double faktor)
        : var(var), targetValue(newVar), faktor(faktor)
{    }

void Animation::step(const double dt) {
    const double a = faktor*dt;
    const double deltaVar = targetValue - (*var);

    *var += a*deltaVar;
}

void Animation::animateToValue(double newVar) {
    this->targetValue = newVar;
}

double Animation::getVar() const { return *var; }

double Animation::getNewVar() const { return targetValue; }

