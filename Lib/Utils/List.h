//
// Created by joao on 28/05/24.
//

#ifndef STUDIOSLAB_LIST_H
#define STUDIOSLAB_LIST_H

#include <list>

namespace Slab {
    template<typename Tp, typename Alloc=std::allocator<Tp>>
    using List = std::list<Tp, Alloc>;
}

#endif //STUDIOSLAB_LIST_H
