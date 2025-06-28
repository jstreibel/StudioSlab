//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_REQUEST_H
#define STUDIOSLAB_REQUEST_H

#include "Utils/String.h"

namespace Slab::Core {

    using FCommand = Str;

    class FRequest {
        FCommand Command;
    public:
        bool operator==(const FRequest&) const;

        bool operator==(const FCommand&) const;
    };

} // Slab::Core

#endif //STUDIOSLAB_REQUEST_H
