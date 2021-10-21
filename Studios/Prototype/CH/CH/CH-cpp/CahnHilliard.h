#ifndef CAHN_HILLIARD_H
#define CAHN_HILLIARD_H

//#include <cmath>
#include "defs.h"

#include "CHData.h"

struct CahnHilliard{

public:
	CahnHilliard(double T, 
				 double A1, double A2, double A3, 
                 double B1, double B2, double B3);

    void operator() (const CHData &phi, CHData &dphidt, const double /* t */);
    void apply_bc(CHData &phi, const double t);

private:
    INLINE double calcCahnHilliard(const CHData &phi, int Cn);
    INLINE double num(const CHData &phi, double T, int Cn);
    INLINE double den(const CHData &phi, double T, int Cn);

public:
    double T;
    double A1, A2, A3, B1, B2, B3;
};

#endif
