#ifndef HAMILTON_H
#define HAMILTON_H

#include "Util/MathTypes.h"
#include "Core/NumericParams.h"


namespace Base {
    template<class FIELD_STATE_TYPE>
    class System {
    public:
        System() {}

        virtual ~System() {}

        //virtual floatt E(const void *phi)                     = 0;
        //virtual floatt Hdens(const void *phi, const floatt x) const = 0;

        //virtual floatt V(const floatt &phi)              const = 0;
        //virtual floatt dVdphi(const floatt &phi)         const = 0;

        virtual FIELD_STATE_TYPE &dtF(const FIELD_STATE_TYPE &in, FIELD_STATE_TYPE &out, Real t, Real dt) = 0;

        FIELD_STATE_TYPE &operator()(const FIELD_STATE_TYPE &in, FIELD_STATE_TYPE &out, Real t, Real dt) {
            return this->dtF(in, out, t, dt);
        }
    };
}

#endif // HAMILTON_H
