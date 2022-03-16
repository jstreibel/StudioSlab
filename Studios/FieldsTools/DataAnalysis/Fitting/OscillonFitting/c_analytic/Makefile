#ANACONDA_PATH = /opt/anaconda

ifdef ANACONDA_PATH
	#$(info Compilando para Anaconda.)
	PYTHONLIB = -lpython3.6m 
	PY_INCLUDE = ${ANACONDA_PATH}/include/python3.6m
	NUMPY_INCLUDE = ${ANACONDA_PATH}/lib/python3.6/site-packages/numpy/core/include
	PYTHON_LIBDIR = -L $(ANACONDA_PATH)/lib
else
	#$(info Compilando para python 3.7 do sistema.)
	PYTHONLIB = -lpython3.7m 
	PY_INCLUDE = /usr/include/python3.7m
	NUMPY_INCLUDE = /usr/lib/python3.7/site-packages/numpy/core/include
	PYTHONLIBDIR =
endif


SWIG = swig
SWIG_FLAGS = -py3

SIM_SOURCE = ${SKYRMECODE}/sim
SOURCE = ./src/fitting.cpp \
	./src/breather.cpp \
	$(SIM_SOURCE)/RtoR/Model/FunctionsCollection/analytic-breather.cpp \
	$(SIM_SOURCE)/RtoR/Model/RtoRFunctionArbitraryCPU.cpp


SIM_HEADERS = ${SKYRMECODE}/sim

CC = g++
CFLAGS = -fPIC -shared 
C_OUTPUT = _py_oscillon.so

SWIG_GENERATED = interface_wrap.cxx
#OBJ = $(C_SRC:.c=.o) $(GERADOS_SWIG:.c=.o)
PY_SWIG_OBJS = $(OBJ) interface.py $(SWIG_GENERATED)
C_LIBS = -lstdc++ $(PYTHONLIB)
SWIG_LINK += -lcpointer.i

all:
	$(SWIG) -c++ -python $(SWIG_FLAGS) interface.i
	$(CC) -O $(CFLAGS) $(SOURCE) $(SWIG_GENERATED) -I$(PY_INCLUDE) $(PYTHON_LIBDIR)  -I$(NUMPY_INCLUDE) -I$(SIM_HEADERS) $(C_LIBS) -o $(C_OUTPUT)
debug:
	$(SWIG) -c++ -python $(SWIG_FLAGS) interface.i
	$(CC) -g -ggdb $(CFLAGS) $(SOURCE) $(SWIG_GENERATED) -I$(PY_INCLUDE) -I$(NUMPY_INCLUDE) -I$(SIM_HEADERS) $(C_LIBS) -o $(C_OUTPUT)
clean:
	rm *.cxx
	rm *.py
	rm *.so
	rm -r __pycache__
