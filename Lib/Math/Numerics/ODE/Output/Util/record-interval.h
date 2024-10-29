#ifndef RECORDINTERVAL_H
#define RECORDINTERVAL_H

#include "Utils/STDLibInclude.h"
#include "Utils/Types.h"


namespace Slab::Math {

    struct RecordInterval {
        RecordInterval(const Real beginT, const Real endT, const Real x);

        Real deltaT() const;

        const Real beginT, endT;
        const Real x;

        void add(Real f, Real t);

        bool contains(const Real T) const;

        const Real &operator()(const Real &t);

        const Vector<Real> &getFVals() const;

        const Vector<Real> &getTVals() const;

    private:
        Vector<Real> fVals;
        Vector<Real> tVals;

    public:
        typedef Vector<RecordInterval>::iterator Iterator;
    };


}

#endif // RECORDINTERVAL_H
