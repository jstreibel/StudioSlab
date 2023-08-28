#include "Animation.h"

Animation::Animation(Real *var, Real newVar, Real faktor)
        : var(var), targetValue(newVar), faktor(faktor)
{    }

void Animation::step(const Real dt) {
    const Real a = faktor*dt;
    const Real deltaVar = targetValue - (*var);

    *var += a*deltaVar;
}

void Animation::animateToValue(Real newVar) {
    this->targetValue = newVar;
}

Real Animation::getVar() const { return *var; }

Real Animation::getNewVar() const { return targetValue; }

