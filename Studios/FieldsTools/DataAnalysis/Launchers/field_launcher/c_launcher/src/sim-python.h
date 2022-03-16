#ifndef BREATHER_H
#define BREATHER_H

#ifdef __cplusplus
extern "C" {
#endif

void setMetaData(double xMin, double xMax);
void setPhi(double *array, int N);
void setdPhidt(double *array, int N);

void run();

#ifdef __cplusplus
}
#endif

#endif
