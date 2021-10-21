#ifndef CAHN_HILLIARD_H
#define CAHN_HILLIARD_H

//#include <cmath>
#include "defs.h"

#include "CHData.h"

struct CahnHilliard{

public:
	CahnHilliard(double T, 
				 double A1, double A2, double A3, 
				 double B1, double B2, double B3,
				 CHData *data) 
				 : T(T), 
				   A1(A1), A2(A2), A3(A3), 
				   B1(B1), B2(B2), B3(B3), 
				   data(data)
	{
		xSwap = new VecDoub(data->x->size());
	}

	VecDoub *xSwap;
	INLINE void step(double timeStep){
	
		/*
		unsigned int lim = data->getLim4();
		for(int Cn=lim; Cn < data->getN()-lim; Cn++)
			(*xSwap)[Cn] = (*data->x)[Cn] - 2.0*timeStep*calcCahnHilliard(Cn);
		*/
		
		for(int Cn=0; Cn < data->getN(); Cn++)
			(*xSwap)[Cn] = (*data->x)[Cn] - 2.0*timeStep*calcCahnHilliard(Cn);

		VecDoub *temp = data->x;
		data->x = xSwap;
		xSwap = temp;
	}

private:
	// TODA A CAHN-HILLIARD ESTA AQUI ABAIXO

	INLINE double calcCahnHilliard(int Cn){
		double c = (*data->x)[Cn];

		const double _num = num(T, Cn);
		const double _den = den(T, c);

		return _num / _den;
	}

	INLINE double num(double T, int Cn){
		double c = (*data->x)[Cn];
		double dc = data->getD1Val(Cn);
		double ddc = data->getD2Val(Cn);
		//double dddc = data->getD3Val(Cn);
		double ddddc = data->getD4Val(Cn);

		/*return
			dc*dc*
			(
			 -0.000171553*T  + 0.000343105*T*c +
					   (0*A1 + 0.000247729*A2 + 0.000990918*A3 + 0.000247729*B2*T)*c*c +
					   (0*A1 - 0.000495459*A2 - 0.003963670*A3 - 0.000495459*B2*T)*c*c*c +
					   (0*A1 + 0.000247729*A2 + 0.004954590*A3 + 0.000247729*B2*T)*c*c*c*c +
					   (0*A1 +           0*A2 - 0.001981840*A3 +           0*B2*T)*c*c*c*c*c
			)

			+

			c*
			(
			ddc*(0.000171553*T    + c*(-0.0000412882*A1 - 0.000123865*A2 - 0.000206441*A3 - 	0.0001715530*T - 0.0000412882*B1*T - 0.0001238650*B2*T)
								+ c*c*( 0.0000825765*A1 + 0.000495459*A2 + 0.001403800*A3 +            	   0*T + 0.0000825765*B1*T + 0.0049545900*B2*T)
							  + c*c*c*(-0.0000412882*A1 - 0.000619323*A2 - 0.003179190*A3 +                0*T - 0.0000412882*B1*T - 0.0006193230*B2*T)
						    + c*c*c*c*(            0*A1 + 0.000247729*A2 + 0.002972750*A3 +                0*T +            0*B1*T + 0.0002477290*B2*T)
						  + c*c*c*c*c*(            0*A1 +           0*A2 - 0.000990918*A3 +                0*T +            0*B1*T +            0*B2*T))
			   -0.010615*(1-c)*(1-c)*c*ddddc
			);*/
        
        return  (-0.000171553*T + 0.000343105*T*c +
                        ( 0.000247729*A2 + 0.000990918*A3 + 0.000247729*B2*T + 0.000990918*B3*T)*c*c +
                        (-0.000495459*A2 - 0.003963670*A3 - 0.000495459*B2*T - 0.003963670*B3*T)*c*c*c +
                        ( 0.000247729*A2 + 0.004954590*A3 + 0.000247729*B2*T + 0.004954590*B3*T)*c*c*c*c +
                        (                - 0.001981840*A3                    - 0.001981840*B3*T)*c*c*c*c*c
                )*dc*dc
        
        +
                c*
                (
                 (0.000171553*T
                    + (-0.0000412882*A1 - 0.0001238650*A2 - 0.000206441*A3 - 0.0001715530*T    - 0.0000412882*B1*T - 0.0001238650*B2*T - 0.000206441*B3*T)*c
                    + ( 0.0000825765*A1 + 0.0004954590*A2 + 0.001403800*A3                     + 0.0000825765*B1*T + 0.0004954590*B2*T + 0.001403800*B3*T)*c*c
                    + (-0.0000412882*A1 - 0.0006193230*A2 - 0.003179190*A3                     - 0.0000412882*B1*T - 0.0006193230*B2*T - 0.003179190*B3*T)*c*c*c
                    + (                   0.0002477290*A2 + 0.002972750*A3                                         + 0.0002477290*B2*T + 0.002972750*B3*T)*c*c*c*c
                    + (                                   - 0.000990918*A3                                                             - 0.000990918*B3*T)*c*c*c*c*c)
                 *ddc + 0.010615*(1.0 - c)*(1.0 - c)*c*ddddc);
    }

	INLINE double den(double T, double c){
		return exp(2987.88/T)*T*(c-1)*(c-1)*c*c;
	}

public:
    double T;
    double A1, A2, A3, B1, B2, B3;
	CHData *data;
};

#endif
