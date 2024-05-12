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

typedef std::valarray<Complex> ComplexArray;
typedef std::vector<Complex>   ComplexVector;

typedef std::valarray<unsigned> UIntArray;
typedef std::vector<int>        IntVector;


template<class T>
typename std::vector<T>     Vector;
typedef std::ostream        OStream;
typedef std::vector<Str>    StrVector;

template <typename T>
class ValarrayWrapper {
private:
    T* data_;
    std::size_t size_;

public:
    // Construct from existing data
    ValarrayWrapper(T* data, std::size_t size) : data_(data), size_(size) {}

    // Mimic the valarray interface you need
    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    std::size_t size() const { return size_; }

    // Example operation: sum
    T sum() const {
        T result = 0;
        for (std::size_t i = 0; i < size_; ++i)
            result += data_[i];
        return result;
    }
};

namespace Utils {

    typedef Real MaxValue_type;
    typedef size_t MaxValue_index;

    typedef std::pair<MaxValue_type, MaxValue_index> MaxInfo;

    MaxInfo GetMax(const RealArray &arr);

}


#endif //STUDIOSLAB_ARRAYS_H
