%module py_oscillon

%include "cpointer.i"
%include "carrays.i"

%array_class(double, doubleArray)

%{
#define SWIG_FILE_WITH_INIT
#include "src/breather.h"
#include "src/fitting.h"
%}

%include "../../../../sources/numpy/tools/swig/numpy.i"
%init %{
        import_array();
%}
%apply (double* IN_ARRAY1, int DIM1) {(double* array, int N)}

%include "src/breather.h"
%include "src/fitting.h"
