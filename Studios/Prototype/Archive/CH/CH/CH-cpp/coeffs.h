#ifndef COEFFS_H
#define COEFFS_H

typedef const double COEFFS;
typedef const unsigned int I;

// FINITE DIFFERENCES COEFFICIENTS
// http://en.wikipedia.org/wiki/Finite_difference_coefficients

// Central coeffs.

//d1 central:
extern I N_d1acc2;
extern I N_d1acc4;
extern I N_d1acc6;
extern I N_d1acc8;
extern COEFFS d1acc2[];
extern COEFFS d1acc4[];
extern COEFFS d1acc6[];
extern COEFFS d1acc8[];
								    				 	 					 			 
//d2 central:
extern I N_d2acc2;
extern I N_d2acc4;
extern I N_d2acc6;
extern I N_d2acc8;
extern COEFFS d2acc2[];
extern COEFFS d2acc4[];
extern COEFFS d2acc6[];
extern COEFFS d2acc8[];
								    													 
//d3 central:
extern I N_d3acc2;
extern I N_d3acc4;
extern I N_d3acc6;
extern COEFFS d3acc2[];
extern COEFFS d3acc4[];
extern COEFFS d3acc6[];

//d4 central:
extern I N_d4acc2;
extern I N_d4acc4;
extern I N_d4acc6;
extern COEFFS d4acc2[];
extern COEFFS d4acc4[];
extern COEFFS d4acc6[];



// Forward & backward coeffs.

// d1 bw fw:
extern I N_d1acc1fb;
extern I N_d1acc2fb;
extern I N_d1acc3fb;
extern I N_d1acc4fb;
extern I N_d1acc5fb;
extern I N_d1acc6fb;
extern COEFFS d1acc1fb[];
extern COEFFS d1acc2fb[];
extern COEFFS d1acc3fb[];
extern COEFFS d1acc4fb[];
extern COEFFS d1acc5fb[];
extern COEFFS d1acc6fb[];

// d2 bw fw:
extern I N_d2acc1fb;
extern I N_d2acc2fb;
extern I N_d2acc3fb;
extern I N_d2acc4fb;
extern I N_d2acc5fb;
extern I N_d2acc6fb;
extern COEFFS d2acc1fb[];
extern COEFFS d2acc2fb[];
extern COEFFS d2acc3fb[];
extern COEFFS d2acc4fb[];
extern COEFFS d2acc5fb[];
extern COEFFS d2acc6fb[];

// d3 bw fw:
extern I N_d3acc1fb;
extern I N_d3acc2fb;
extern I N_d3acc3fb;
extern I N_d3acc4fb;
extern I N_d3acc5fb;
extern I N_d3acc6fb;
extern COEFFS d3acc1fb[];
extern COEFFS d3acc2fb[];
extern COEFFS d3acc3fb[];
extern COEFFS d3acc4fb[];
extern COEFFS d3acc5fb[];
extern COEFFS d3acc6fb[];

// d4 bw fw:
extern I N_d4acc1fb;
extern I N_d4acc2fb;
extern I N_d4acc3fb;
extern I N_d4acc4fb;
extern I N_d4acc5fb;
extern COEFFS d4acc1fb[];
extern COEFFS d4acc2fb[];
extern COEFFS d4acc3fb[];
extern COEFFS d4acc4fb[];
extern COEFFS d4acc5fb[];

#endif
