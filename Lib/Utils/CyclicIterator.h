//
// Created by joao on 5/29/24.
//

#ifndef STUDIOSLAB_CYCLICITERATOR_H
#define STUDIOSLAB_CYCLICITERATOR_H

#include "Arrays.h"

namespace Slab {
    template <typename T>
    class CyclicIterator {
    public:
        CyclicIterator(const std::vector<T>& elements) : elements_(elements), current_index_(0) {}

        T next() {
            if (elements_.empty()) {
                throw std::out_of_range("The container is empty.");
            }
            T element = elements_[current_index_];
            current_index_ = (current_index_ + 1) % elements_.size();
            return element;
        }

        void reset() {
            current_index_ = 0;
        }

    private:
        const std::vector<T>& elements_;
        size_t current_index_;
    };

}

#endif //STUDIOSLAB_CYCLICITERATOR_H
