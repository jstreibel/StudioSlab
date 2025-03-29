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

    template <typename T, typename Predicate>
    typename List<T>::iterator FindFirst_If(List<T>& lst, Predicate pred) {
        for(auto it = lst.begin(); it != lst.end(); ++it) {
            if(pred(*it)) {
                return it;
            }
        }
        return lst.end();
    }

    template <typename T>
    bool MoveToFront(List<T>& lst, const T& value) {
        auto it = std::ranges::find(lst.begin(), lst.end(), value);
        if (it != lst.end()) {
            lst.splice(lst.begin(), lst, it);
            return true;
        }
        return false;
    }

    template <typename T>
    bool MoveToFront(List<T>& list, const typename List<T>::iterator& it) {
        if (it != list.end()) {
            list.splice(list.begin(), list, it);
            return true;
        }
        return false;
    }
}

#endif //STUDIOSLAB_LIST_H
