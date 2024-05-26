//
// Created by joao on 11/4/21.
//

#include "Category.h"


namespace Slab::Math {

    std::ostream &operator<<(std::ostream &os, const Category &obj) {
        return os << obj.ToString();
    }


}
