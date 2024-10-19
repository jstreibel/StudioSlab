//
// Created by joao on 11/6/21.
//

#ifndef STUDIOSLAB_SET_H
#define STUDIOSLAB_SET_H

#include <set>

namespace Slab::Math::Base {

    /**
     * From Wikipedia studies:
     * - In mathematics, a set is a collection of elements;
     * - Each element can be any _mathematical object_;

     * @tparam Element
     */
    template<class Element> // ta certo isso?
    class Set {
    public:
        typedef std::set<Element> Collection;

        virtual auto getElements() -> Collection = 0;
    };

}

#endif //STUDIOSLAB_SET_H
