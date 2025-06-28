//
// Created by joao on 10/21/24.
//

#ifndef STUDIOSLAB_UNIQUEOBJECT_H
#define STUDIOSLAB_UNIQUEOBJECT_H

#include "Utils/Numbers.h"
#include "Utils/String.h"

namespace Slab::Core {

    using IdType = UInt;

    class UniqueObject {
        static CountType Count;
        IdType Id;

    public:
        [[nodiscard]] IdType GetId() const;
        [[nodiscard]] Str GetIdString() const;
        [[nodiscard]] Str AddUniqueIdToString(const Str&) const;

        UniqueObject();

    };

} // Slab::Core

#endif //STUDIOSLAB_UNIQUEOBJECT_H
