//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_MESSAGE_H
#define STUDIOSLAB_MESSAGE_H

#include "Utils/String.h"

namespace Slab::Core {

    using Content = Str;

    class Message {
        Content message;
    public:
        Message(Content);
    };

} // Slab::Core

#endif //STUDIOSLAB_MESSAGE_H
