#ifndef LINK_CPP
#define LINK_CPP

#include "link.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

#include "./CH/CahnHilliard.h"
#include "./CH/CHData.h"

static CahnHilliard *ch;

int InitCH(double *CHDataPtr, int N, double h, double T, 
			double A1, double A2, double A3, 
			double B1, double B2, double B3,
			int acc){
				
	try{
		CHData::DConfig config;
		switch(acc){
		case 1:
			config = CHData::acc1;
			break;
		case 2:
			config = CHData::acc2;
			break;
		case 3:
			config = CHData::acc3;
			break;
		case 4:
			config = CHData::acc4;
			break;
		case 5:
			config = CHData::acc5;
			break;
		case 6:
			config = CHData::acc6;
			break;
		case 7:
			config = CHData::acc7;
			break;
		default:
			config = CHData::acc1;
			printf("acc%i nao existe. Usando 1. Na proxima vez use um valor de 1 ate 7.", acc);
		}
		CHData *data = new CHData(N, h, config);
		
		VecDoub &x = *(data->x);
		for(int i=0; i<N; i++)
			x[i] = CHDataPtr[i];
		
		ch = new CahnHilliard(T, A1, A2, A3, B1, B2, B3, data);
		cout << "Inicializado com sucesso." << endl;
	}
	catch(...){
	    cout << "Erro na inicializacao." << endl;
		return -1;
	}
	
	return 1;
}

double *X(){	
	VecDoub &v = *(ch->data->x);
	
	double *x = &v[0];
	return x;	
}

void stepCH(int nSteps, double timeStep){
	using namespace std;
	clock_t begin = clock();

	double total = 0.0;
	for(int i=0; i<nSteps; i++){
		ch->step(timeStep);
		
		if(!(i % 1000)){			
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			
			if(elapsed_secs > 2){
				total += elapsed_secs;
				printf("\nelapsed: %.2fs, step: %i / %i", total, i, nSteps);
				begin = clock();
			}
		}
	}

	printf("\n---\nTOTAL: %.2fsec   --->   %i steps\n\n", total, nSteps);
	fflush(stdout);
}

void setT(double T){
	ch->T = T;
}
#endif
