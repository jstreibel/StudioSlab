#include "fractal_calc.h"

#include <cmath>
#include <iostream>

#include <map>
#include <string>

#include <boost/functional/hash.hpp>

struct Index{
	int i, j, k;

	std::size_t hash() const
    {
        std::size_t seed = 0;

        boost::hash_combine(seed, i);
        boost::hash_combine(seed, j);
		boost::hash_combine(seed, k);

        return seed;
    }
};

bool operator < (const Index &a, const Index &b){
	//return a.toString() < b.toString();

	return a.hash() < b.hash();
}

int _n = 0;
double *invec = NULL;
double Lmin, Lmax;

double *xData = NULL;
double *yData = NULL;
double *zData = NULL;

int Init(int n_points){
	_n = n_points;
	if(xData != NULL) delete xData;
	if(yData != NULL) delete yData;
	if(zData != NULL) delete zData;

	xData = new double[_n];
	yData = new double[_n];
	zData = new double[_n];

	return 1;
}
int Finish(){
	return 1;
}

void set_xData(double *array, int N){
	if(N != _n) throw "Error: array dim is different than expected.";

	for(int i=0; i<N; i++) xData[i] = array[i];
}
void set_yData(double *array, int N){
	if(N != _n) throw "Error: array dim is different than expected.";

	for(int i=0; i<N; i++) yData[i] = array[i];
}
void set_zData(double *array, int N){
	if(N != _n) throw "Error: array dim is different than expected.";

	for(int i=0; i<N; i++) zData[i] = array[i];
}

int getN(double eps){
	int i, j, k;
	Index index;

	std::map<Index, int> boxes;

	for(int p=0; p<_n; p++){
		double x = xData[p], y = yData[p], z = zData[p];
		i = int(floor(x/eps));
		j = int(floor(y/eps));
		k = int(floor(z/eps));

		boxes[{i, j, k}]++;
	}

	/*for(auto el : boxes){
		const Index &i = el.first;
		int count = el.second;
	}*/

	return boxes.size();
}
