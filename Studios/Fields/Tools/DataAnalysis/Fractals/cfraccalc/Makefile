PYTHONLIB = -lpython3.7m

SWIG = swig
SWIG_FLAGS = -py3
PYTHONLIBDIR =

#SIM_SOURCE = ../../../sim/
SOURCE = fractal_calc.cpp
#	$(SIM_SOURCE)/boundaries/analytic-breather.cpp \
	$(SIM_SOURCE)/cpu/field-1d-cpu.cpp
#SIM_HEADERS = ../../../

CC = g++
PY_INCLUDE = /usr/include/python3.7m
NUMPY_INCLUDE = /usr/lib/python3.7/site-packages/numpy/core/include
CFLAGS = -fPIC -shared
C_OUTPUT = _fraccalc.so

SWIG_GENERATED = interface_wrap.cxx
#OBJ = $(C_SRC:.c=.o) $(GERADOS_SWIG:.c=.o)
#PY_SWIG_OBJS = $(OBJ) interface.py $(SWIG_GENERATED)
C_LIBS = -lstdc++ $(PYTHONLIB)
SWIG_LINK += -lcpointer.i

all:
	$(SWIG) -c++ -python $(SWIG_FLAGS) interface.i
	$(CC) $(CFLAGS) $(SOURCE) $(SWIG_GENERATED) -I$(PY_INCLUDE) \
			-I$(NUMPY_INCLUDE) -I$(SIM_HEADERS) $(C_LIBS) -o $(C_OUTPUT)
clean:
	rm *.cxx
	rm fraccalc.py
	rm *.so
