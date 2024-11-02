#pragma once

#include "defs.h"
#include "coeffs.h"

#define PERIODIC_BC

class CHData
{
public:
    enum DConfig {
        acc1 = 1,
        acc2,
        acc3,
        acc4,
        acc5,
        acc6,
        acc7,
    } config;

    CHData(const size_t nSitios, const double h, DConfig config);

    CHData() : h(0), v() {}

    inline double val(int Cn) const;

    double getD1Val(int Cn) const;
    double getD2Val(int Cn) const;
    double getD3Val(int Cn) const;
    double getD4Val(int Cn) const;

    const double h;
	double invh;

    unsigned int getLim4(){
        return lim4;
    }

    CHData& operator + (const CHData &data){
        for(size_t i=0; i<data.v.size(); i++)
            this->v[i] += data.v[i];

        return *this;
    }

    CHData& operator * (const double &a){
        for(double &el : this->v)
            el *= a;

        return *this;
    }

    VecDoub::iterator begin() { return v.begin(); }
    VecDoub::iterator end() { return v.end(); }

private:
    VecDoub v;

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
};
