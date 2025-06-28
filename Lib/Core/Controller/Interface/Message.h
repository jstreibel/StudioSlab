//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_MESSAGE_H
#define STUDIOSLAB_MESSAGE_H

#include "Utils/String.h"

namespace Slab::Core {

    using FContent = Str;

    class FMessage {
        FContent Message;
    public:
        explicit FMessage(FContent);
    };

} // Slab::Core

#endif //STUDIOSLAB_MESSAGE_H
