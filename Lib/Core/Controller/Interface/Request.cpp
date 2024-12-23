//
// Created by joao on 10/14/24.
//

#include "Request.h"

namespace Slab::Core {

    bool Request::operator==(const Request &other) {
        return command==other.command;
    }

    bool Request::operator==(const Command &received_command) {
        return command==received_command;
    }
} // Slab::Core