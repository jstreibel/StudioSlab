//
// Created by joao on 28/05/24.
//

#ifndef STUDIOSLAB_LIST_H
#define STUDIOSLAB_LIST_H

#include <list>

namespace Slab {
    template<typename Tp, typename Alloc=std::allocator<Tp>>
    using List = std::list<Tp, Alloc>;

    template<typename Tp>
    using BasicList = std::initializer_list<Tp>;

    template<typename Tp>
    List<Tp> Extend(List<Tp> list, List<Tp> more_items) {

        List<Tp> extended_list(list);
        for(auto &item : more_items)
            extended_list.push_back(item);

        return extended_list;
    }
}

#endif //STUDIOSLAB_LIST_H
