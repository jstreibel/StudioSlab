#include "Animation_depr.h"

Animation_depr::Animation_depr(Real *var, Real newVar, Real faktor)
        : var(var), targetValue(newVar), faktor(faktor)
{    }

void Animation_depr::step(const Real dt) {
    const Real a = faktor*dt;
    const Real deltaVar = targetValue - (*var);

    *var += a*deltaVar;
}

void Animation_depr::animateToValue(Real newVar) {
    this->targetValue = newVar;
}

Real Animation_depr::getVar() const { return *var; }

Real Animation_depr::getNewVar() const { return targetValue; }

