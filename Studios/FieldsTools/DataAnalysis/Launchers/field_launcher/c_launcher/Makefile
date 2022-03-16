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


SWIG = swig-2
SWIG_FLAGS = -py3

SIM_SOURCE = ${SKYRMECODE}/sim/sim
SOURCE = ./src/sim-python.cpp \
	$(SIM_SOURCE)/../backend/glut-backend.cpp \
	$(SIM_SOURCE)/../io/output-info.cpp \
	$(SIM_SOURCE)/../io/output.cpp \
	$(SIM_SOURCE)/../io/output-history.cpp \
	$(SIM_SOURCE)/../io/render/output-opengl.cpp \
	$(SIM_SOURCE)/../io/render/plotting-utils.cpp \
	$(SIM_SOURCE)/../io/render/graph.cpp \
	$(SIM_SOURCE)/../io/render/animation.cpp \
	$(SIM_SOURCE)/cpu/hamilton-1d-weq-cpu.cpp \
	$(SIM_SOURCE)/hamiltonians/hamilton-absphi.cpp \
	$(SIM_SOURCE)/hamiltonians/hamilton-abs-phi-perturbed.cpp \
	$(SIM_SOURCE)/sims/simulation.cpp \
	$(SIM_SOURCE)/sims/sim-any-field.cpp \
	$(SIM_SOURCE)/boundaries/analytic-breather.cpp \
	$(SIM_SOURCE)/boundaries/shockwave.cpp \
	$(SIM_SOURCE)/cpu/field-1d-cpu.cpp \
	$(SIM_SOURCE)/params.cpp \
	$(SIM_SOURCE)/integrator.cpp
	

SIM_HEADERS = ${SKYRMECODE}/sim

CC = g++
CFLAGS = -fPIC -shared -fopenmp
C_OUTPUT = _sim_python.so
C_LIBS = -lstdc++ $(PYTHONLIB) -lGL -lGLEW -lglut -lboost_program_options -lgomp

SWIG_GENERATED = interface_wrap.cxx
#OBJ = $(C_SRC:.c=.o) $(GERADOS_SWIG:.c=.o)
PY_SWIG_OBJS = $(OBJ) interface.py $(SWIG_GENERATED)
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
