//
// Created by joao on 23/09/23.
//

#ifndef STUDIOSLAB_ARRAYS_H
#define STUDIOSLAB_ARRAYS_H

#include "Numbers.h"
#include "String.h"

#include <vector>
#include <valarray>

typedef std::valarray<Real>    RealArray;
typedef std::vector<Real>      RealVector;
typedef const RealArray        RealArray_I;
typedef RealArray              RealArray_O;
typedef RealArray              VecFloat_IO;

typedef std::valarray<Complex> ComplexArray;
typedef std::vector<Complex>   ComplexVector;

typedef std::valarray<unsigned> UIntArray;
typedef std::vector<int>        IntVector;


template<class T>
typename std::vector<T>     Vector;
typedef std::ostream        OStream;
typedef std::vector<Str> StrVector;


typedef std::vector<Point2D> PointContainer;

#endif //STUDIOSLAB_ARRAYS_H
