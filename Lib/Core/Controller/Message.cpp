//
// Created by joao on 10/14/24.
//

#include "Message.h"

#include <utility>


namespace Slab::Core {
    FMessage::FMessage(FContent cont) : Message(std::move(cont)) {

    }
} // Core::Slab