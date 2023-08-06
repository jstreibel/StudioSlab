#ifndef C_INTERFACE_H
#define C_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

int Init(int n_points);
int Finish();

void set_xData(double *array, int N);
void set_yData(double *array, int N);
void set_zData(double *array, int N);

int getN(double eps);

#ifdef __cplusplus
}
#endif

#endif
