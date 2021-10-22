#ifndef RECORDINTERVAL_H
#define RECORDINTERVAL_H

#include "Lib/Util/Workaround/STDLibInclude.h"

struct RecordInterval{
	RecordInterval(const double beginT, const double endT, const double x);

	double deltaT() const;

	const double beginT, endT;
	const double x;

	void add(double f, double t);

	bool contains(const double T) const;

	const double & operator() (const double &t);

	const std::vector<double> &getFVals() const;
	const std::vector<double> &getTVals() const;

private:
    std::vector<double> fVals;
    std::vector<double> tVals;

public:
	typedef std::vector<RecordInterval>::iterator Iterator;
};

#endif // RECORDINTERVAL_H
