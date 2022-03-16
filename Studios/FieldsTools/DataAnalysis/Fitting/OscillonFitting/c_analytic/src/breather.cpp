#include "breather.h"
#include "RtoR/Model/FunctionsCollection/AnalyticOscillon.cpp"

std::vector<AnalyticOscillon*> breathers;

int newBreather(int N, double xMin, double xMax, bool mirrored, bool anti_sym){

	AnalyticOscillon *breather = new AnalyticOscillon(N, xMin, xMax, mirrored, anti_sym);

	breathers.push_back(breather);

	return breathers.size() - 1;
}

void finishBreather(int myIndex){
	AnalyticOscillon *breather = breathers[myIndex];

	// The line below is commented out bc it won't work since we need to keep the index mapping correct.
	// breathers.erase(breathers.begin()+myIndex);

	delete breather;
 }

double *get(int myIndex, double t, double v, double V, double alpha, double xTranslate,
							 double l)
{
	AnalyticOscillon *breather = breathers[myIndex];

	breather->get(t, v, V, alpha, xTranslate, l);

	return &breather->phi[0];
}

double getPhiVal(double t, double x, double v){
	return double(getPhiValue(t, x, v));
}
double getdPhidtVal(double t, double x, double v){
	return double(getdPhidtValue(t, x, v));
}

double getPsiVal(double t, double x, double v, double u){
	return double(psi(t, x, v, u));
}
double getdPsidtVal(double t, double x, double v, double u){
	return double(dpsi(t, x, v, u));
}
