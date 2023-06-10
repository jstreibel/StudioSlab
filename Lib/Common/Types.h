//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_TYPES_H
#define STUDIOSLAB_TYPES_H

#include "DeviceConfig.h"
#include "NativeTypes.h"
#include "STDLibInclude.h"

typedef floatt              Real;

typedef std::size_t         PosInt;
typedef int_least64_t       BigInt;

typedef std::vector<Real>   VecFloat;
typedef const VecFloat      VecFloat_I;
typedef VecFloat            VecFloat_O;
typedef VecFloat            VecFloat_IO;

template<class T1, class T2>
struct Pair {
    T1 first;
    T2 second;
};

typedef Pair<int, int>      IntPair;
typedef std::pair<const class DiscreteSpace*, const class DiscreteSpace*> DiscreteSpacePair;

typedef std::ostream        OStream;
typedef std::string         String;
typedef std::stringstream   StringStream;
typedef std::vector<String> StrVector;



#endif //STUDIOSLAB_TYPES_H
