#include "sim-python.h"

#include "backend/glut-backend.h"
#include "sim/sims/sim-any-field.h"
#include "sim/cpu/field-1d-cpu.h"

double *_Phi = nullptr;
double *_dPhidt = nullptr;
unsigned _N = 0;
double _xMin, _xMax;

Field_CPU *field = nullptr;

void setMetaData(double xMin, double xMax){
	_xMin = xMin;
	_xMax = xMax;
}

void setPhi(double *array, int N){
	_Phi = array;
	_N = N;
}
void setdPhidt(double *array, int N){
	if(!(N==_N)) throw "Unmatching array size for Phi and dPhidt.";

	_dPhidt = array;
}

void run(){
	field = new Field_CPU(_N, (_xMax-_xMin)/double(_N), _xMin);
	for(int i=0; i<_N; i++){
		field->setPhi(i, _Phi[i]);
		field->setDPhidT(i, _dPhidt[i]);
	}

	double tMax = 100.0;
	Params p(_N, _xMax-_xMin, tMax, 0.1);
	unsigned nThreads = 1;
	bool doOutputToFile = false;

	// Quando a simulacao eh destruida ela destroi o campo.
	SimAnyField sim = SimAnyField(p, field, doOutputToFile, nThreads);

	GLUTBackend *backend = GLUTBackend::getSingleton();

	backend->run(&sim);
}
