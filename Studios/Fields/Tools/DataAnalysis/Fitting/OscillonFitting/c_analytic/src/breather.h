#ifndef BREATHER_H
#define BREATHER_H

#ifdef __cplusplus
extern "C" {
#endif

int newBreather(int N, double xMin, double xMax, bool mirrored, bool anti_sym);
void finishBreather(int myIndex);
double *get(int myIndex, double t, double v, double V, double alpha,
            double xTranslate, double l);

double getPhiVal(double t, double x, double v);
double getdPhidtVal(double t, double x, double v);

double getPsiVal(double t, double x, double v, double u);
double getdPsidtVal(double t, double x, double v, double u);


#ifdef __cplusplus
}
#endif

#endif
