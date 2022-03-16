#include "c_interface.h"
#include "../../../core-sim/sim/boundaries/analytic-breather.h"

#include <iostream>

int N = 0;
double *invec = NULL;
double Lmin, Lmax;

double *myVec = NULL;

int Init(){
	invec = NULL;
	return 1;
}

int Finish(){
	return 1;
}

double *getOsc(double T, double v, double V, double phase, double xTranslate,
							 double l, bool mirror)
{
	AnalyticBreather(myVec, N, T, v, V, phase, xTranslate, l, Lmin, Lmax, mirror);

	return myVec;
}
int getN() {
	return N;
}
double getDif(double T, double v, double V, double phase, double xTranslate, double l, bool mirror){
	assert(N>0 && myVec != NULL);

	AnalyticBreather(myVec, N, T, v, V, phase, xTranslate, l, Lmin, Lmax, mirror);

	double sum = 0;
	for(size_t i=0; i<N; i++){
		const double dif = myVec[i]-invec[i];
		sum += dif*dif;
	}

	return sum;
}

void setMetaData(double _Lmin, double _Lmax){
	Lmin = _Lmin;
	Lmax = _Lmax;

}
void setData(double *_invec, int _N){
	if(N != _N){
		if(myVec != NULL) delete [] myVec;
		myVec = new double[_N];
		//std::cout << "Alloc'd " << _N*sizeof(double) << " bytes of mem for oscillon vector." << std::endl;
	}

	invec = (double*)_invec;
	N = _N;
}

double computeLeastSquares(double T, double alpha, double v, double V, double l, double xTranslate, bool mirror, bool anti_sym){
	AnalyticBreather(myVec, N, T, v, V, alpha, xTranslate, l, Lmin, Lmax, mirror);

	double sum = 0;
	for(size_t i=0; i<N; i++){
		const double dif = myVec[i]-invec[i];
		sum += dif*dif;
	}

	return sum/double(N);
}

double computeDiffSquares(double T, double alpha, double v, double V, double l, double xTranslate, bool mirror, bool anti_sym){
	AnalyticBreather(myVec, N, T, v, V, alpha, xTranslate, l, Lmin, Lmax, mirror);

	double sum = 0;
	for(size_t i=0; i<N; i++){
		const double dif = myVec[i]-invec[i];
		sum += dif;
	}

	return sum*sum/double(N);
}
