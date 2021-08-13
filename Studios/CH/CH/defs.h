#pragma once

#ifndef DEFS_H
#define DEFS_H

#include <limits>
#include <vector>

#include <cstdlib>
#include <cstdio>
#include <math.h>

#ifndef INLINE
#ifdef MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline

#endif
#endif

typedef std::vector<double> VecDoub;
typedef VecDoub VecDoub_O;
typedef const VecDoub VecDoub_I;
typedef VecDoub VecDoub_IO;
typedef std::vector<std::vector<double> > MatDoub;

typedef int Int;
typedef double Doub;

inline double SIGN (const double &a, const double &b) { return ((b) >= 0.0 ? fabs(a) : -fabs(a)); }
inline double MIN (const double &a, const double &b) { return ((a < b) ? a : b); }
inline double MAX (const double &a, const double &b) { return ((a > b) ? a : b); }
inline double sqr (const double &v) { return v*v; }

//void throwEx (const char *message);

extern "C" inline void resizeMat(MatDoub &mat, int H, int W);

//void throwEx (const char *message) { printf(message, NULL); exit(-1); }

void resizeMat(MatDoub &mat, int H, int W){
	MatDoub tempmat;

	size_t ni = mat.size();
	size_t nj = ni == 0 ? 0 : mat[0].size();

	for (size_t i=0; i<ni; i++)
		for (size_t j=0; j<nj; j++)
			tempmat[i][j] = mat[i][j];

	mat.resize(H);

	for (int i=0; i < H; ++i)
		mat[i].resize(W);

	for(size_t i=0; i<ni; i++)
		for(size_t j=0; j<nj; j++)
			mat[i][j] = tempmat[i][j];
}


#endif
