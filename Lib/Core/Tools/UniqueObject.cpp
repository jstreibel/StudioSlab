//
// Created by joao on 10/21/24.
//

#include "UniqueObject.h"

namespace Slab::Core {

    CountType UniqueObject::Count = 0;

    UniqueObject::UniqueObject() : Id(++Count) {

    }

    Str UniqueObject::AddUniqueIdToString(const Str& str) const {
        return str + "##" + ToStr(Id);
    }

    IdType UniqueObject::GetId() const {
        return Id;
    }

    Str UniqueObject::GetIdString() const {
        return ToStr(GetId());
    }

} // Slab::Core