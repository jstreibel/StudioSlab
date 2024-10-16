//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_REQUEST_H
#define STUDIOSLAB_REQUEST_H

#include "Utils/String.h"

namespace Slab::Core {

    using Command = Str;

    class Request {
        Command command;
    public:
        bool operator==(const Request&);

        bool operator==(const Command&);
    };

} // Slab::Core

#endif //STUDIOSLAB_REQUEST_H
