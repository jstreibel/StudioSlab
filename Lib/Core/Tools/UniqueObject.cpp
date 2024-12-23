//
// Created by joao on 10/21/24.
//

#include "UniqueObject.h"

namespace Slab::Core {

    Count UniqueObject::count = 0;

    UniqueObject::UniqueObject() : id(++count) {

    }

    Str UniqueObject::unique(const Str& str) const {
        return str + "##" + ToStr(id);
    }

    Id UniqueObject::get_id() const {
        return id;
    }

    Str UniqueObject::get_id_str() const {
        return Slab::ToStr(get_id());
    }

} // Slab::Core