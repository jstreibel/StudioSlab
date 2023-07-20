#ifndef HAMILTON_H
#define HAMILTON_H

#include "Common/Types.h"


namespace Base {
    template<typename EqStateType>
    class DifferentialEquation {
    public:


        DifferentialEquation() {}

        virtual ~DifferentialEquation() {}

        virtual EqStateType &dtF(const EqStateType &in, EqStateType &out, Real t, Real dt) = 0;

        EqStateType &operator()(const EqStateType &in, EqStateType &out, Real t, Real dt) {
            return this->dtF(in, out, t, dt);
        }

        virtual void startStep(Real t, Real dt) {};
        virtual void finishStep(Real t, Real dt) {};

    };
}

#endif // HAMILTON_H
