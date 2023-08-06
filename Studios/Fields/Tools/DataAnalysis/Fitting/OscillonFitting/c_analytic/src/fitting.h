#ifndef FITTING_H
#define FITTING_H

#ifdef __cplusplus
extern "C" {
#endif

int NewFit();
void DelFit(int fitIndex);

void clearAllData(int fitIndex);

int addData(int fitIndex, double *array, int N, double t, double xMin, double xMax);

void clearAllData(int fitIndex);

double *getOscFromFit(int fitIndex, int index, double alpha, double v,
			   double V, double l, double xTranslate);

int getNFromFit(int fitIndex, int oscIndex);

double computeAbsDiffAvg(int fitIndex, double alpha, double v, double V, double l,
                           double xTranslate);

double computeLeastSquares(int fitIndex, double alpha, double v, double V,
                           double l, double xTranslate);

#ifdef __cplusplus
}
#endif

#endif
