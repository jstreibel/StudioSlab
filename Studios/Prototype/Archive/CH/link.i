%module CH

%include "cpointer.i"
%include "carrays.i"

%array_class(double, doubleArray)

%{
#include "link.h"
%}

//#include "link.h"

extern int InitCH(double *CHDataPtr, int N, double h, double T, 
				   double A1, double A2, double A3,
				   double B1, double B2, double B3,
				   int acc);
				   
extern double *X();

extern void stepCH(int nSteps, double timeStep);

extern void setT(double T);