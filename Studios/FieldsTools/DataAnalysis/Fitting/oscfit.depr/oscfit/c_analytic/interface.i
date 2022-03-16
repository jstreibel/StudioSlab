%module oscfit

%include "cpointer.i"
%include "carrays.i"

%array_class(double, doubleArray)

%{
#define SWIG_FILE_WITH_INIT
#include "c_interface.h"
%}

%include "../../../../sources/numpy/tools/swig/numpy.i"
%init %{
        import_array();
%}

%apply (double* IN_ARRAY1, int DIM1) {(double* array, int N)}
%include "c_interface.h"
