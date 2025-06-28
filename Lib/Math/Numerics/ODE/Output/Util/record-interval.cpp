#include "record-interval.h"

//#include <assert.h>


namespace Slab::Math {

    RecordInterval::RecordInterval(const DevFloat beginT, const DevFloat endT,
                                   const DevFloat x)
            : beginT(beginT), endT(endT), x(x) {
        //assert(endT>beginT);
    }

    DevFloat RecordInterval::deltaT() const { return endT - beginT; }

    void RecordInterval::add(DevFloat f, DevFloat t) {
        //assert((t>=beginT) && (t<=endT));

        fVals.push_back(f);
        tVals.push_back(t);
    }

    bool RecordInterval::contains(const DevFloat T) const {
        return (T >= beginT) && (T <= endT);
    }

    const DevFloat &RecordInterval::operator()(const DevFloat &) {
        //assert((t>=beginT) && (t<=endT));

        throw "Not implemented";
    }

    const Vector<DevFloat> &RecordInterval::getFVals() const { return fVals; }

    const Vector<DevFloat> &RecordInterval::getTVals() const { return tVals; }


}