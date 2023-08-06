%module sim_python

%include "cpointer.i"
%include "carrays.i"

%array_class(double, doubleArray)

%{
#define SWIG_FILE_WITH_INIT
#include "src/sim-python.h"
%}

%include "../../../../sources/numpy/tools/swig/numpy.i"
%init %{
        import_array();
%}
%apply (double* IN_ARRAY1, int DIM1) {(double* array, int N)}

%include "src/sim-python.h"
