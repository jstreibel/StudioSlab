#ifndef HAMILTON_H
#define HAMILTON_H

#include "Common/Types.h"


namespace Base {
    template<class FIELD_STATE_TYPE>
    class DifferentialEquation {
    public:
        DifferentialEquation() {}

        virtual ~DifferentialEquation() {}

        virtual FIELD_STATE_TYPE &dtF(const FIELD_STATE_TYPE &in, FIELD_STATE_TYPE &out, Real t, Real dt) = 0;

        FIELD_STATE_TYPE &operator()(const FIELD_STATE_TYPE &in, FIELD_STATE_TYPE &out, Real t, Real dt) {
            return this->dtF(in, out, t, dt);
        }

        virtual void startStep(Real t, Real dt) {};
        virtual void finishStep(Real t, Real dt) {};

    };
}

#endif // HAMILTON_H
