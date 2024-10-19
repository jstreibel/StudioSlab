//
// Created by joao on 7/26/24.
//

#ifndef STUDIOSLAB_MAP_H
#define STUDIOSLAB_MAP_H

#include <map>

namespace Slab {

    template<typename T, typename U>
    using Map = std::map<T, U>;

    template<typename T, typename U>
    using HashMap = std::unordered_map<T, U>;

    template<typename T, typename U>
    bool Contains(Map<T,U> map, T key){
        return map.find(key) != map.end();
    }


}

#endif //STUDIOSLAB_MAP_H
