//
// Created by joao on 10/21/24.
//

#ifndef STUDIOSLAB_UNIQUEOBJECT_H
#define STUDIOSLAB_UNIQUEOBJECT_H

#include "Utils/Numbers.h"
#include "Utils/String.h"

namespace Slab::Core {

    using Id = UInt;

    class UniqueObject {
        static Count count;
        Id id;

    public:
        Id get_id() const;
        Str get_id_str() const;
        Str unique(const Str&) const;

        UniqueObject();


    };

} // Slab::Core

#endif //STUDIOSLAB_UNIQUEOBJECT_H
