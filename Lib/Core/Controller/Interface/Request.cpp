//
// Created by joao on 10/14/24.
//

#include "Request.h"

namespace Slab::Core {

    bool FRequest::operator==(const FRequest &other) const
    {
        return Command == other.Command;
    }

    bool FRequest::operator==(const FCommand &ReceivedCommand) const
    {
        return Command == ReceivedCommand;
    }
} // Slab::Core