#ifndef C_INTERFACE_H
#define C_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

int Init();
int Finish();
double *getOsc(double T, double v, double V, double phase, double xTranslate, double l, bool mirror);
int getN();
double getDif(double T, double v, double V, double phase, double xTranslate, double l, bool mirror);

double computeLeastSquares(double T, double alpha, double v, double V, double l, double xTranslate, bool mirror, bool anti_sym);
double computeDiffSquares(double T, double alpha, double v, double V, double l, double xTranslate, bool mirror, bool anti_sym);

void setMetaData(double Lmin, double Lmax);
void setData(double *array, int N);

#ifdef __cplusplus
}
#endif

#endif
