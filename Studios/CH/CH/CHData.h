#pragma once

#include "defs.h"
#include "coeffs.h"

#define PERIODIC_BC

class CHData
{
private:
	// FINITE DIFFERENCES COEFFICIENTS (http://en.wikipedia.org/wiki/Finite_difference_coefficients)
	unsigned int N_d1c;
	unsigned int N_d2c;
	unsigned int N_d3c;
	unsigned int N_d4c;
	unsigned int lim1, lim2, lim3, lim4;

	unsigned int N_d1fb;
	unsigned int N_d2fb;
	unsigned int N_d3fb;
	unsigned int N_d4fb;

	const double *d1c;
	const double *d2c;
	const double *d3c;
	const double *d4c;
	
	const double *d1fb;
	const double *d2fb;
	const double *d3fb;
	const double *d4fb;
	
	unsigned int *perIndex;

public:
	unsigned int getLim4(){
		return lim4;
	}
	enum DConfig {
		acc1 = 1,
		acc2,
		acc3,
		acc4,
		acc5,
		acc6,
		acc7,
	} config;

	CHData(const int nSitios, const double h, DConfig config) : config(config), h(h), invh(1/h)
	{
		x = new VecDoub(nSitios);
		N = (int)x->size();

		switch(config)
		{
		case acc1:
			N_d1c = N_d1acc2;	// eh acc2 mesmo
			N_d2c = N_d2acc2;	// eh acc2 mesmo
			N_d3c = N_d3acc2;	// eh acc2 mesmo
			N_d4c = N_d4acc2;	// eh acc2 mesmo
			d1c =   d1acc2;     // eh acc2 mesmo
			d2c =   d2acc2;		// eh acc2 mesmo
			d3c =   d3acc2;		// eh acc2 mesmo
			d4c =   d4acc2;		// eh acc2 mesmo

			N_d1fb = N_d1acc1fb;
			N_d2fb = N_d2acc1fb;
			N_d3fb = N_d3acc1fb;
			N_d4fb = N_d4acc1fb;
			d1fb =   d1acc1fb;
			d2fb =   d2acc1fb;
			d3fb =   d3acc1fb;
			d4fb =   d4acc1fb;

			break;
		case acc2:
			N_d1c = N_d1acc2;
			N_d2c = N_d2acc2;
			N_d3c = N_d3acc2;
			N_d4c = N_d4acc2;
			d1c =   d1acc2;
			d2c =   d2acc2;
			d3c =   d3acc2;
			d4c =   d4acc2;

			N_d1fb = N_d1acc2fb;
			N_d2fb = N_d2acc2fb;
			N_d3fb = N_d3acc2fb;
			N_d4fb = N_d4acc2fb;
			d1fb =   d1acc2fb;
			d2fb =   d2acc2fb;
			d3fb =   d3acc2fb;
			d4fb =   d4acc2fb;

			break;
		case acc3:
			N_d1c = N_d1acc4;   // eh acc4 mesmo
			N_d2c = N_d2acc4;   // eh acc4 mesmo
			N_d3c = N_d3acc4;   // eh acc4 mesmo
			N_d4c = N_d4acc4;   // eh acc4 mesmo
			d1c =   d1acc4;     // eh acc4 mesmo
			d2c =   d2acc4;		// eh acc4 mesmo
			d3c =   d3acc4;		// eh acc4 mesmo
			d4c =   d4acc4;		// eh acc4 mesmo

			N_d1fb = N_d1acc3fb;
			N_d2fb = N_d2acc3fb;
			N_d3fb = N_d3acc3fb;
			N_d4fb = N_d4acc3fb;
			d1fb =   d1acc3fb;
			d2fb =   d2acc3fb;
			d3fb =   d3acc3fb;
			d4fb =   d4acc3fb;

			break;
		case acc4:
			N_d1c = N_d1acc4;
			N_d2c = N_d2acc4;
			N_d3c = N_d3acc4;
			N_d4c = N_d4acc4;
			d1c =   d1acc4;
			d2c =   d2acc4;
			d3c =   d3acc4;
			d4c =   d4acc4;

			N_d1fb = N_d1acc4fb;
			N_d2fb = N_d2acc4fb;
			N_d3fb = N_d3acc4fb;
			N_d4fb = N_d4acc4fb;
			d1fb =   d1acc4fb;
			d2fb =   d2acc4fb;
			d3fb =   d3acc4fb;
			d4fb =   d4acc4fb;

			break;
		case acc5:
			N_d1c = N_d1acc6;   // eh acc6 mesmo
			N_d2c = N_d2acc6;	// eh acc6 mesmo
			N_d3c = N_d3acc6;	// eh acc6 mesmo
			N_d4c = N_d4acc6;	// eh acc6 mesmo
			d1c =   d1acc6;     // eh acc6 mesmo
			d2c =   d2acc6;		// eh acc6 mesmo
			d3c =   d3acc6;		// eh acc6 mesmo
			d4c =   d4acc6;		// eh acc6 mesmo

			N_d1fb = N_d1acc5fb;
			N_d2fb = N_d2acc5fb;
			N_d3fb = N_d3acc5fb;
			N_d4fb = N_d4acc5fb;
			d1fb =   d1acc5fb;
			d2fb =   d2acc5fb;
			d3fb =   d3acc5fb;
			d4fb =   d4acc5fb;

			break;
		case acc6:
			N_d1c = N_d1acc6;
			N_d2c = N_d2acc6;
			N_d3c = N_d3acc6;
			N_d4c = N_d4acc6;
			d1c =   d1acc6;
			d2c =   d2acc6;
			d3c =   d3acc6;
			d4c =   d4acc6;

			N_d1fb = N_d1acc6fb;
			N_d2fb = N_d2acc6fb;
			N_d3fb = N_d3acc6fb;
			N_d4fb = N_d4acc5fb; // eh acc5 mesmo
			d1fb =   d1acc6fb;
			d2fb =   d2acc6fb;
			d3fb =   d3acc6fb;
			d4fb =   d4acc5fb; // eh acc5 mesmo

			break;
		case acc7:
			N_d1c = N_d1acc8; // eh acc8 mesmo (nao tem 7)
			N_d2c = N_d2acc8; // eh acc8 mesmo (nao tem 7)
			N_d3c = N_d3acc6; // eh acc6 mesmo (nao tem 8 nem 7)
			N_d4c = N_d4acc6; // eh acc6 mesmo (nao tem 8 nem 7)
			d1c =   d1acc8;	// eh acc8 mesmo (nao tem 7)
			d2c =   d2acc8; // eh acc8 mesmo (nao tem 7)
			d3c =   d3acc6; // eh acc6 mesmo (nao tem 8 nem 7)
			d4c =   d4acc6;	// eh acc6 mesmo (nao tem 8 nem 7)

			N_d1fb = N_d1acc6fb; // eh acc6 mesmo (so vai ate 6)
			N_d2fb = N_d2acc6fb; // eh acc6 mesmo (so vai ate 6)
			N_d3fb = N_d3acc6fb; // eh acc6 mesmo (so vai ate 6)
			N_d4fb = N_d4acc5fb; // eh acc5 mesmo (so vai ate 5)
			d1fb =   d1acc6fb; // eh acc6 mesmo (so vai ate 6)
			d2fb =   d2acc6fb; // eh acc6 mesmo (so vai ate 6)
			d3fb =   d3acc6fb; // eh acc6 mesmo (so vai ate 6)
			d4fb =   d4acc5fb; // eh acc5 mesmo (so vai ate 5)

			break;
		}

		lim1 = (N_d1c-1)/2;
		lim2 = (N_d2c-1)/2;
		lim3 = (N_d3c-1)/2;
		lim4 = (N_d4c-1)/2;
		
		perIndex = new unsigned int[N_d4c];
	}

	inline double val(int Cn){
		#ifdef PERIODIC_BC
		if(Cn < 0) Cn = N+Cn;
		else if(Cn > N-1) Cn = Cn-N;
		#endif
			
		return (*x)[Cn];
	}

	INLINE double getD1Val(int Cn){
		const unsigned int lim = lim1;
		const unsigned int nc = N_d1c;
		const double invden = invh;
		double       num = 0.0;
		const double *dc = d1c;		
		
		#ifndef PERIODIC_BC
		const unsigned int nfb = N_d1fb;
		const int BW_SIGN = -1;
		const double *dfb = d1fb;
		if     (Cn < lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += dfb[i]+val(Cn+i);
		}
		else if(Cn > N-1-lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += BW_SIGN*dfb[i]+val(Cn-i);
		}
		else
		#endif
		{
			for(unsigned int i = 0; i<nc; i++)
				num += dc[i]*val(Cn-lim+i);				
		}
		
		return invden*num;
	}
	INLINE double getD2Val(int Cn){
		const unsigned int lim = lim2;
		const unsigned int nc = N_d2c;
		const double invden = invh*invh;
		double       num = 0.0;
		const double *dc = d2c;
		
		#ifndef PERIODIC_BC
		const unsigned int nfb = N_d2fb;
		const int BW_SIGN = 1;
		const double *dfb = d2fb;
		if     (Cn < lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += dfb[i]+val(Cn+i);
		}
		else if(Cn > N-1-lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += BW_SIGN*dfb[i]+val(Cn-i);
		}
		else
		#endif
		{
			for(unsigned int i = 0; i<nc; i++)
				num += dc[i]*val(Cn-lim+i);
		}
		
		return invden*num;
	}
	INLINE double getD3Val(int Cn){
		const unsigned int lim = lim3;
		const unsigned int nc = N_d3c;
		
		const double invden = invh*invh*invh;
		double       num = 0.0;

		const double *dc = d3c;

		#ifndef PERIODIC_BC
		const unsigned int nfb = N_d3fb;
		const int BW_SIGN = -1;
		const double *dfb = d3fb;
		if     (Cn < lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += dfb[i]+val(Cn+i);
		}
		else if(Cn > N-1-lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += BW_SIGN*dfb[i]+val(Cn-i);
		}
		else
		#endif
		{
			for(unsigned int i = 0; i<nc; i++)
				num += dc[i]*val(Cn-lim+i);
		}
		
		return invden*num;
	}
	INLINE double getD4Val(int Cn){
		const unsigned int lim = lim4;
		const unsigned int nc = N_d4c;
		const double invden = invh*invh*invh*invh;
		double       num = 0.0;
		const double *dc = d4c;

		#ifndef PERIODIC_BC
		const unsigned int nfb = N_d4fb;
		const int BW_SIGN = 1;
		const double *dfb = d4fb;
		if     (Cn < lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += dfb[i]+val(Cn+i);
		}
		else if(Cn > N-1-lim){
			for(unsigned int i = 0; i<nfb; i++)
				num += BW_SIGN*dfb[i]+val(Cn-i);
		}
		else
		#endif
		{
			for(unsigned int i = 0; i<nc; i++)
				num += dc[i]*val(Cn-lim+i);
		}
		
		return invden*num;
	}

	INLINE int getN() const { return N;}

	double h;
	double invh;
	VecDoub *x; // tem que ser ponteiro para fazer swap.

private:
	int N;
};