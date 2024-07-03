//
// Created by joao on 7/2/24.
//

#ifndef STUDIOSLAB_ZIP_H
#define STUDIOSLAB_ZIP_H

#include "Arrays.h"

namespace Slab {

    template<typename T1, typename T2>
    auto Zip(const Vector<T1>& vec1, const Vector<T2>& vec2) {
        Vector<std::tuple<T1, T2>> zipped;
        auto size = std::min(vec1.size(), vec2.size());
        for (std::size_t i = 0; i < size; ++i) {
            zipped.emplace_back(vec1[i], vec2[i]);
        }
        return zipped;
    }

}

#endif //STUDIOSLAB_ZIP_H
