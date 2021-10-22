#include "record-interval.h"

//#include <assert.h>

RecordInterval::RecordInterval(const double beginT, const double endT,
                               const double x)
    : beginT(beginT), endT(endT), x(x)
{
    //assert(endT>beginT);
}

double RecordInterval::deltaT() const { return endT - beginT; }

void RecordInterval::add(double f, double t){
    //assert((t>=beginT) && (t<=endT));

    fVals.push_back(f);
    tVals.push_back(t);
}

bool RecordInterval::contains(const double T) const {
    return (T>=beginT) && (T<=endT);
}

const double & RecordInterval::operator() (const double &){
    //assert((t>=beginT) && (t<=endT));

    throw "Not implemented";
}

const std::vector<double> &RecordInterval::getFVals() const { return fVals; }
const std::vector<double> &RecordInterval::getTVals() const { return tVals; }
