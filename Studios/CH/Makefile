PYTHONLIB=-lpython3.7m

# Just in case:
# export PATH=/Users/jstreibel/Library/Enthought/Canopy_64bit/User/bin:/usr/local/cuda/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/opt/X11/bin

# MAC:
#PYTHONLIBDIR=-L/Users/jstreibel/Library/Enthought/Canopy_64bit/User/lib
#SWIG = swig

# LINUX:
SWIG = swig-2
SWIG_FLAGS = -py3
PYTHONLIBDIR =

# COMUM:
CC = g++
#PY_INCLUDE = /Applications/Canopy.app/Contents/include/python2.7
PY_INCLUDE = /usr/include/python3.7m
CFLAGS = -c -fPIC

FONTE = link.cpp
GERADOS_SWIG = link_wrap.cxx
#OBJ = $(FONTE_C:.c=.o) $(GERADOS_SWIG:.c=.o)
PY_SWIG_OBJS = $(OBJ) CH.py $(GERADOS_SWIG)
ENDFLAGS = -lstdc++
SWIG_FLAGS += -lcpointer.i

all:
	echo ----------------------------------------------------------------------------
	$(SWIG) -c++ -python $(SWIG_FLAGS) link.i
	echo    ---- fim swig ----
	$(CC) -fPIC -shared $(FONTE) $(GERADOS_SWIG) -I$(PYTHONINCDIR) $(PY_INCLUDE) $(ENDFLAGS) -o _CH.so $(PYTHONLIBDIR) $(PYTHONLIB)
clean:
	rm -rf *o $(PY_SWIG_OBJS)
	rm -rf *~
	rm -rf *.c~
	rm -rf *.i~
	rm -rf *.pyc
