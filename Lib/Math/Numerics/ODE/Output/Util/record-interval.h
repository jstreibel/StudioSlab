#ifndef RECORDINTERVAL_H
#define RECORDINTERVAL_H

#include "Utils/STDLibInclude.h"
#include "Utils/Types.h"


namespace Slab::Math {

    struct RecordInterval {
        RecordInterval(const DevFloat beginT, const DevFloat endT, const DevFloat x);

        DevFloat deltaT() const;

        const DevFloat beginT, endT;
        const DevFloat x;

        void add(DevFloat f, DevFloat t);

        bool contains(const DevFloat T) const;

        const DevFloat &operator()(const DevFloat &t);

        const Vector<DevFloat> &getFVals() const;

        const Vector<DevFloat> &getTVals() const;

    private:
        Vector<DevFloat> fVals;
        Vector<DevFloat> tVals;

    public:
        typedef Vector<RecordInterval>::iterator Iterator;
    };


}

#endif // RECORDINTERVAL_H
