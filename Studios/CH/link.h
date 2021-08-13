#ifndef LINK_H
#define LINK_H


#ifdef __cplusplus
extern "C" {
#endif

int coisa();

int InitCH(double *CHDataPtr, int N, double h, double T, 
			double A1, double A2, double A3, 
			double B1, double B2, double B3,
			int acc);
double *X();

void stepCH(int nSteps, double timeStep);

void setT(double T);

#ifdef __cplusplus
}
#endif

#endif
