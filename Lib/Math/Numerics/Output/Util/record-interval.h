#ifndef RECORDINTERVAL_H
#define RECORDINTERVAL_H

#include "Utils/STDLibInclude.h"
#include "Utils/Types.h"

struct RecordInterval{
	RecordInterval(const Real beginT, const Real endT, const Real x);

	Real deltaT() const;

	const Real beginT, endT;
	const Real x;

	void add(Real f, Real t);

	bool contains(const Real T) const;

	const Real & operator() (const Real &t);

	const std::vector<Real> &getFVals() const;
	const std::vector<Real> &getTVals() const;

private:
    std::vector<Real> fVals;
    std::vector<Real> tVals;

public:
	typedef std::vector<RecordInterval>::iterator Iterator;
};

#endif // RECORDINTERVAL_H
