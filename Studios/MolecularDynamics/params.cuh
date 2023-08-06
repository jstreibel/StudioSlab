#ifndef PARAMS_CUH
#define PARAMS_CUH

/**** Nao tocar nisso ****/
#define PERIODIC 1
#define RIGID 2
#define LEAPFROG 1
#define EULER 2
/****** Obrigado :) ******/

#define INT_METHOD EULER
#define BOUNDARY PERIODIC
#define EXTERNAL_FIELD false

#define BOX_SIDE 300.0
#define BOX_HALF_SIDE 150.0
#define Nparticles 2*2048
#define	P 128
#define R_ BOX_HALF_SIDE
#define DT 1.0e-0

#define EPS 3.e0

#endif
