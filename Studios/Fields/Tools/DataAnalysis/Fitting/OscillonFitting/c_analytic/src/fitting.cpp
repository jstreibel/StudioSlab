#include "fitting.h"
#include <vector>
#include <map>
#include "RtoR/Model/RtoRFunctionArbitraryCPU.h"
#include "RtoR/Model/FunctionsCollection/analytic-breather.h"
#include <math.h>

class Fitting {
public:
	Fitting() : timeStamps(0), inputData(0), analyticBreathers(0) {}
	~Fitting(){
		timeStamps.clear();
	    inputData.clear();
		analyticBreathers.clear();
	}
	int addData(double *array, int N, double t, double xMin, double xMax){
	    // Dentro desta biblioteca, todos os calculos sao feitos proximos do zero (tanto)
	    // o oscillon analitico quanto a solucao numerica sao trazidos de volta para a origem.

	    // Obs.: Field_CPU(N, h, xLeftBoundary)
	    const double h = (xMax-xMin)/N;
	    const double xLeftBoundary = (xMax-xMin)*.5;
	    Field_CPU *field = new Field_CPU(N, h, xLeftBoundary);
	    for(int i=0; i<N; i++) field->phi[i] = array[i];

		// o oscillon original, que fica no intervalo [0, 1] eh espelhado aqui.
		const bool mirrored = true; // (queremos fitting em termos do oscillon original)
	    AnalyticBreather *analytic = new AnalyticBreather(N, xMin, xMax, mirrored);
	    //cout << "New analytic breather added in region (" << xMin << ", " << xMax << ")" << endl;

	    inputData.push_back(field);
	    analyticBreathers.push_back(analytic);
	    if(timeStamps.size() > 0) assert(t > timeStamps.back());
	    timeStamps.push_back(t);

	    for(auto &val : field->phi) assert(!isnan(val));

	    int myIndex = inputData.size()-1;

	    return myIndex;
	}

	void clearAllData(){
		timeStamps.clear();
		analyticBreathers.clear();
		inputData.clear();
	}

	void _do_assertions() {
		/* Assert all times have corresponding values. */
		assert(timeStamps.size() == inputData.size() && inputData.size() == analyticBreathers.size());

		const int M = inputData.size();
		for(int i=0; i<M; i++){
			const int N = inputData[i]->N;

			/* Assert all corresponding fields have same size. */
			assert(analyticBreathers[i]->N == N);

			/* Assert all time stamps are in time order */
			if(i<M-1) assert(timeStamps[i] < timeStamps[i+1]);
		}
	}

	double *getOscFromFit(int oscIndex, double alpha, double v,
						  double V, double l, double xTranslate){
		const double t = timeStamps[oscIndex];
		AnalyticBreather *breather = analyticBreathers[oscIndex];
		breather->get(t, v, V, alpha, xTranslate, l);

		return &breather->phi[0];
	}

	int getNFromFit(int oscIndex){
		return analyticBreathers[oscIndex]->N;
	}

	double computeAbsDiffAvg(double alpha, double v, double V, double l,
							 double xTranslate)
	{
		_do_assertions();

		int count = 0;
		const int M = inputData.size();
		double totalSum = 0.;
		double innerSumAnalytic = 0.;
		double innerSumSim = 0.;
		for(int i=0; i<M; i++){
			const double t = timeStamps[i];
			const Field_CPU *inField = inputData[i];
			const Field_CPU *analyticField = &analyticBreathers[i]->get(t, v, V, alpha, xTranslate, l);

			const int N = inField->N;
			double partSum = 0.;
			for(int j=0; j<N; j++){
				double val = analyticField->phi[j] - inField->phi[j];
				partSum += abs(val);
				count++;

				innerSumAnalytic += abs(analyticField->phi[j]);
				innerSumSim += abs(inField->phi[j]);
			}

			totalSum += partSum;
		}

		return totalSum/count;
	}

	double computeLeastSquares(double alpha, double v, double V, double l,
							   double xTranslate)
	{
		_do_assertions();

		const int M = inputData.size();

		double totalSum = 0.;
		double innerSumAnalytic = 0.;
		double innerSumSim = 0.;
		for(int i=0; i<M; i++){
			const double t = timeStamps[i];
			const Field_CPU *inField = inputData[i];
			const Field_CPU *analyticField = &analyticBreathers[i]->get(t, v, V, alpha, xTranslate, l);

			const int N = inField->N;
			double partSum = 0.;
			for(int j=0; j<N; j++){
				double val = analyticField->phi[j]-inField->phi[j];
				partSum += val*val;

				innerSumAnalytic += abs(analyticField->phi[j]);
				innerSumSim += abs(inField->phi[j]);
			}
		totalSum += partSum;
		}

		cout << "\n\tinnerSumAnalytic = " << innerSumAnalytic << "\n\tinnerSumSim = " << innerSumSim << endl;

		return totalSum;
	}

private:
	Vector<floatt> timeStamps;
	Vector<Field_CPU*> inputData;
	Vector<AnalyticBreather*> analyticBreathers;
};

std::map<int, Fitting*> fits;

int NewFit() {
	int index = fits.size();

	fits[index] = new Fitting();

	return index;
}

void DelFit(int index) {
	// std::map destroi objetos automaticamente.
	fits.erase(index);
}

int addData(int fitIndex, double *array, int N, double t, double xMin, double xMax){
	Fitting *fit = fits[fitIndex];

    return fit->addData(array, N, t, xMin, xMax);
}

void clearAllData(int fitIndex){
	Fitting *fit = fits[fitIndex];

	fit->clearAllData();
}

double *getOscFromFit(int fitIndex, int oscIndex, double alpha, double v,
					  double V, double l, double xTranslate){
	Fitting *fit = fits[fitIndex];

	return fit->getOscFromFit(oscIndex, alpha, v, V, l, xTranslate);
}

int getNFromFit(int fitIndex, int oscIndex){
	Fitting *fit = fits[fitIndex];

	return fit->getNFromFit(oscIndex);
}

double computeAbsDiffAvg(int fitIndex, double alpha, double v, double V, double l,
						 double xTranslate)
{
	Fitting *fit = fits[fitIndex];

	return fit->computeAbsDiffAvg(alpha, v, V, l, xTranslate);
}

double computeLeastSquares(int fitIndex, double alpha, double v, double V, double l,
						   double xTranslate)
{
	Fitting *fit = fits[fitIndex];

	return fit->computeLeastSquares(alpha, v, V, l, xTranslate);
}
