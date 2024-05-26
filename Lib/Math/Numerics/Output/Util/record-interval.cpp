#include "record-interval.h"

//#include <assert.h>


namespace Slab::Math {

    RecordInterval::RecordInterval(const Real beginT, const Real endT,
                                   const Real x)
            : beginT(beginT), endT(endT), x(x) {
        //assert(endT>beginT);
    }

    Real RecordInterval::deltaT() const { return endT - beginT; }

    void RecordInterval::add(Real f, Real t) {
        //assert((t>=beginT) && (t<=endT));

        fVals.push_back(f);
        tVals.push_back(t);
    }

    bool RecordInterval::contains(const Real T) const {
        return (T >= beginT) && (T <= endT);
    }

    const Real &RecordInterval::operator()(const Real &) {
        //assert((t>=beginT) && (t<=endT));

        throw "Not implemented";
    }

    const Vector<Real> &RecordInterval::getFVals() const { return fVals; }

    const Vector<Real> &RecordInterval::getTVals() const { return tVals; }


}