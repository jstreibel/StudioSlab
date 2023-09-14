//
// Created by joao on 11/4/21.
//

#include "Category.h"

std::ostream &operator<<(std::ostream &os, const Category &obj) {
    return os << obj.ToString();
}
