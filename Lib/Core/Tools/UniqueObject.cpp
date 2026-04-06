//
// Created by joao on 10/21/24.
//

#include "UniqueObject.h"

namespace Slab::Core {

    CountType FUniqueObject::Count = 0;

    FUniqueObject::FUniqueObject() : Id(++Count) {

    }

    Str FUniqueObject::AddUniqueIdToString(const Str& str) const {
        return str + "##" + ToStr(Id);
    }

    IdType FUniqueObject::GetId() const {
        return Id;
    }

    Str FUniqueObject::GetIdString() const {
        return ToStr(GetId());
    }

} // Slab::Core
